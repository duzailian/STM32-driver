#include "include.h"
#include "reg.h"

#define TX_GPIO_IOMODE gpio_output_10M
#define RX_GPIO_IOMODE gpio_input

#define TX_GPIO_CNF gpio_af_opp
#define RX_GPIO_CNF gpio_float

#define ERRO_FLG \
  (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)

typedef struct {
  uint16_t us_len;  // received data length
  uint8_t auc_data[0];
} __st_recv_packet_t;

typedef const struct {
  gpio_cfg_t rx;
  gpio_cfg_t tx;
} st_gpio_t;

typedef struct {
  OS_Q* p_q;
  char* name;
  OS_MSG_QTY q_size;
} st_q_t;

typedef struct {
  uint8_t* puc_buffer;
  uint8_t* puc_index;  // buffer index
  uint16_t us_len;     // buffer size
  uint16_t us_num;     // buffers total number
} st_buffer_t;

typedef const struct {
  st_drv_oper_t st_oper;  // operate API for usart
  USART_TypeDef* USARTx;
  st_gpio_t* pst_gpio;
  const st_dma_parm_t* pst_dma;
  st_q_t st_q;
  st_int_parm_t st_int;  // information for interrupt
  st_buffer_t st_buffer;

  uint32_t ul_rcc_enr;
  uint32_t ul_bandrate;
} st_info_t;

static int __usart_send(void* __self, uint8_t* puc_data, size_t sz_len);
static int __usart_recv(void* __self, uint8_t* puc_buffer, size_t sz_len);
static int __usart_close(void* __self);
static int __ISR(void* pst_info);

#define GPIO_INFO(port, __mode)                                \
  {                                                            \
    .gpiox = GPIO_USART##port##_##__mode##_PORT,               \
    .pinx = GPIO_USART##port##_##__mode##_PIN,                 \
    .io_mode = __mode##_GPIO_IOMODE, .cnf = __mode##_GPIO_CNF, \
    .af_remap = GPIO_USART##port##_REMAP_EN,                   \
    .af_remap_mask = GPIO_USART##port##_REMAP_MASK,            \
  }

#define INFO(ch)                                       \
  [usart##ch] = {                                      \
      .st_oper =                                       \
          {                                            \
              .write = __usart_send,                   \
              .read = __usart_recv,                    \
              .close = __usart_close,                  \
          },                                           \
      .USARTx = USART##ch,                             \
      .pst_gpio = &st_u##ch##gpio,                     \
      .ul_rcc_enr = RCC_APB1ENR(USART##ch),            \
      .ul_bandrate = USART##ch##_BAUDRATE,             \
      .st_int =                                        \
          {                                            \
              .en_int = en_USART##ch##_int,            \
              .func = __ISR,                           \
              .p_arg = (void*)&ast_info[usart##ch],    \
          },                                           \
      .st_q =                                          \
          {                                            \
              .p_q = &u##ch##_q,                       \
              .q_size = USART##ch##_BUFFER_NUM,        \
              .name = "USART" #ch,                     \
          },                                           \
      .st_buffer =                                     \
          {                                            \
              .puc_buffer = &auc_u##ch##_buffer[0][0], \
              .puc_index = &uc_u##ch##index,           \
              .us_len = USART##ch##_BUFFER_SZ,         \
              .us_num = USART##ch##_BUFFER_NUM,        \
          },                                           \
      .pst_dma = &st_u##ch##dma,                       \
  }

#define ADD_CHANNEL(ch)                                           \
  static OS_Q u##ch##_q = {0};                                    \
  static uint8_t auc_u##ch##_buffer[USART##ch##_BUFFER_NUM]       \
                                   [USART##ch##_BUFFER_SZ] = {0}; \
  static uint8_t uc_u##ch##index = 0;                             \
  static st_gpio_t st_u##ch##gpio = {                             \
      .rx = GPIO_INFO(ch, RX),                                    \
      .tx = GPIO_INFO(ch, TX),                                    \
  };                                                              \
  static const st_dma_parm_t st_u##ch##dma = {                    \
      .en_dma = USART##ch##_DMAX,                                 \
      .en_dma_ch = USART##ch##_DMA_CH,                            \
      .en_tr_dir = en_to_mem,                                     \
      .per_addr = (uint32_t)&USART##ch->DR,                       \
      .mem_addr = (uint32_t)&auc_u##ch##_buffer[0][offsetof(      \
          __st_recv_packet_t, auc_data)],                         \
      .us_len = sizeof(auc_u##ch##_buffer[0]),                    \
      .complete_cb = __ISR,                                       \
      .complete_cb_arg = (void*)&ast_info[usart##ch],             \
  };

static st_info_t ast_info[];

ADD_CHANNEL(2)  // usart2

static st_info_t ast_info[] = {
    INFO(2),  // usart2
};

static void __init_reg(st_info_t* pst_info) {
  USART_TypeDef* USARTx = pst_info->USARTx;
  uint32_t fck = 0;
  RCC_ClocksTypeDef RCC_Clocks;

  RCC_GetClocksFreq(&RCC_Clocks);
  if (USARTx == USART1)
    fck = RCC_Clocks.PCLK2_Frequency;
  else
    fck = RCC_Clocks.PCLK1_Frequency;

  USARTx->CR1 = CR1_IDLEIE_Set | CR1_TE_Set | CR1_RE_Set | CR1_PEIE_Set;
  USARTx->CR3 = CR3_DMAR_Set | CR3_EIE_Set;
  USARTx->BRR = fck / pst_info->ul_bandrate;
  USARTx->CR1 |= CR1_UE_Set;
  return;
}

static void __init_gpio(st_gpio_t* pst_info) {
  init_gpio(&pst_info->rx);
  init_gpio(&pst_info->tx);
  return;
}

static void inline __init_rcc(st_info_t* pst_info) {
  if (pst_info->USARTx == USART1)
    RCC->APB2ENR |= RCC_APB2ENR(USART1);
  else
    RCC->APB1ENR |= pst_info->ul_rcc_enr;

  return;
}

static void __init_q(const st_q_t* pst_q) {
  OS_ERR err;
  OSQCreate(pst_q->p_q, pst_q->name, pst_q->q_size, &err);
  if (OS_ERR_NONE != err) {
    LOG_ERR("usart q creat failed(%d)!", err);
  }
  return;
}

static st_info_t* __init_channel(st_info_t* pst_info) {
  __init_rcc(pst_info);
  __init_reg(pst_info);
  __init_gpio(pst_info->pst_gpio);
  __init_q(&pst_info->st_q);
  start_dma(pst_info->pst_dma);
  reg_int_func(&pst_info->st_int);
  return pst_info;
}

static int __usart_send(void* __self, uint8_t* puc_data, size_t sz_len) {
  st_info_t* self = (st_info_t*)__self;
  int ret = 0;
  USART_TypeDef* USARTx = self->USARTx;
  size_t i = 0;

  for (i = 0; i < sz_len; i++) {
    while (!(USARTx->SR & USART_FLAG_TXE))
      ;
    USARTx->DR = puc_data[i];
  }
  return ret;
}

static int __usart_recv(void* __self, uint8_t* puc_data, size_t sz_len) {
  st_info_t* self = (st_info_t*)__self;
  const st_q_t* pst_q = &self->st_q;
  int ret = 0;
  OS_ERR err;
  OS_MSG_SIZE msg_size = 0;
  __st_recv_packet_t* pst_ret = NULL;

  pst_ret = OSQPend(pst_q->p_q, 0, OS_OPT_PEND_BLOCKING, &msg_size, NULL, &err);
  if (OS_ERR_NONE != err) {
    LOG_ERR("usart q wait failed(%d)!", err);
  }
  ret = (pst_ret->us_len > sz_len) ? sz_len : pst_ret->us_len;

  memcpy(puc_data, pst_ret->auc_data, ret);
  return ret;
}

static int __usart_close(void* __self) {
#if 0
  st_info_t *self = (st_info_t *)__self;
  int ret = 0;

  return ret;
#else
  return 0;
#endif
}

static void __resart_dma(const st_buffer_t* pst_buffer,
                         const st_dma_parm_t* pst_dma) {
  st_dma_parm_t st_dma = *pst_dma;
  uint8_t uc_index = *pst_buffer->puc_index;

  uc_index++;
  uc_index %= pst_buffer->us_num;

  st_dma.mem_addr = (uint32_t)((uint32_t)uc_index * pst_buffer->us_len +
                               offsetof(__st_recv_packet_t, auc_data) +
                               pst_buffer->puc_buffer);
#if 0// USART_DBG
  printf("st_dma.mem_addr:0x%x\r\n", st_dma.mem_addr);
#endif
  start_dma(&st_dma);
  *pst_buffer->puc_index = uc_index;
  return;
}

static void post_data(st_info_t* self, uint8_t uc_data) {
  const st_buffer_t* pst_buffer = &self->st_buffer;
  const st_q_t* pst_q = &self->st_q;
  size_t sz_index = (uint32_t)*pst_buffer->puc_index * pst_buffer->us_len;
  OS_ERR err;
  __st_recv_packet_t* pst_ret =
      (__st_recv_packet_t*)&pst_buffer->puc_buffer[sz_index];

  pst_ret->us_len = get_transterred_size(self->pst_dma);
  OSQPost(pst_q->p_q, pst_ret, sizeof(pst_ret), OS_OPT_POST_FIFO, &err);
  if (OS_ERR_NONE != err) {
    LOG_ERR("usart data post error(%d)!", err);
  }
  return;
}

static int __ISR(void* __self) {
  st_info_t* self = (st_info_t*)__self;
  USART_TypeDef* USARTx = self->USARTx;
  uint32_t status = USARTx->SR;

  if (status & ERRO_FLG) {  // received error!
    LOG_ERR("usart received error(%d)!", status & ERRO_FLG);
    goto Return;
  }
  if (status & USART_FLAG_IDLE) {  // one packet received
    const st_buffer_t* pst_buffer = &self->st_buffer;

    post_data(self, USARTx->DR);
    __resart_dma(pst_buffer, self->pst_dma);
  }
Return:
  return 0;
}

extern st_drv_oper_t* open_usart(en_usart_t channel) {
  if ((NULL == &ast_info[channel].USARTx) ||
      (channel >= sizeof_array(ast_info))) {
    LOG_ERR("usart%d open fail!", channel);
    return NULL;
  }
  return (st_drv_oper_t*)__init_channel(&ast_info[channel]);
}

extern int fputc(int ch, FILE* stream) {
  USART_TypeDef* USARTx = ast_info[PRINT_USART].USARTx;
  while (!(USARTx->SR & USART_FLAG_TXE))
    ;
  USARTx->DR = ch;
  return ch;
}

#if USART_DBG
extern void test_usart(void) {
  static uint8_t auc_tmp[100];
  st_drv_oper_t* usart = open_usart(usart2);

  while (1) {
    usart->read(usart, auc_tmp, sizeof(auc_tmp));
    printf("recv:%s\r\n", auc_tmp);
    if (0 == usart->write(usart, "This is a usart test!\r\n",
                          strlen("This is a usart test!\r\n"))) {
      LOG_INFO("usart send test succeed!");
    } else {
      LOG_ERR("usart send test fail!");
    }
  }
}
#endif
