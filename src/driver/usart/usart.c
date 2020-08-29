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
} __st_packet_t;

typedef const struct {
  gpio_cfg_t rx;
  gpio_cfg_t tx;
} st_gpio_t;

#ifndef BOOT_PRJ
typedef struct {
  OS_Q* p_q;
  char* name;
  OS_MSG_QTY q_size;
} st_q_t;
#else
typedef struct {
  list_t p_q;
  __st_packet_t st_pkt;
} st_q_t;
#endif

typedef struct {
  uint8_t* puc_buffer;
  uint8_t* puc_index;  // buffer index
  uint16_t us_len;     // buffer size
  uint16_t us_num;     // buffers total number
} st_buffer_t;

typedef const struct {
  st_drv_if_t st_if;  // operate API for usart
  USART_TypeDef* USARTx;
  st_gpio_t* pst_gpio;
  const st_dma_parm_t* pst_dma;
#ifndef BOOT_PRJ
  st_q_t st_q;
#else
  list_t* pst_q;
#endif
  st_int_parm_t st_int;  // information for interrupt
  st_buffer_t st_buffer;

  uint32_t ul_rcc_enr;
  uint32_t ul_bandrate;
} st_info_t;

static int __usart_send(void* __self, uint8_t* puc_data, size_t sz_len);
static int __usart_recv(void* __self, uint8_t* puc_buffer, size_t sz_len);
static int __usart_close(void* __self);
static int __ISR(void* self);

#define GPIO_INFO(__port, __mode)                              \
  {                                                            \
    .port = GPIO_USART##__port##_##__mode##_PORT,              \
    .pinx = GPIO_USART##__port##_##__mode##_PIN,               \
    .io_mode = __mode##_GPIO_IOMODE, .cnf = __mode##_GPIO_CNF, \
    .af_remap = GPIO_USART##__port##_REMAP_EN,                 \
    .af_remap_mask = GPIO_USART##__port##_REMAP_MASK,          \
  }

#define RCC_USART1_ENR RCC_APB2ENR(USART1)
#define RCC_USART2_ENR RCC_APB1ENR(USART2)
#define RCC_USART3_ENR RCC_APB1ENR(USART3)
#define RCC_USART4_ENR RCC_APB1ENR(UART4)
#define RCC_USART5_ENR RCC_APB1ENR(UART5)

#ifndef BOOT_PRJ
#define INFO(ch)                                       \
  [usart##ch] = {                                      \
      .st_if =                                         \
          {                                            \
              .write = __usart_send,                   \
              .read = __usart_recv,                    \
              .close = __usart_close,                  \
          },                                           \
      .USARTx = USART##ch,                             \
      .pst_gpio = &st_u##ch##gpio,                     \
      .ul_rcc_enr = RCC_USART##ch##_ENR,               \
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
#else
#define INFO(ch)                                       \
  [usart##ch] = {                                      \
      .st_if =                                         \
          {                                            \
              .write = __usart_send,                   \
              .read = __usart_recv,                    \
              .close = __usart_close,                  \
          },                                           \
      .USARTx = USART##ch,                             \
      .pst_gpio = &st_u##ch##gpio,                     \
      .ul_rcc_enr = RCC_USART##ch##_ENR,               \
      .ul_bandrate = USART##ch##_BAUDRATE,             \
      .st_int =                                        \
          {                                            \
              .en_int = en_USART##ch##_int,            \
              .func = __ISR,                           \
              .p_arg = (void*)&ast_info[usart##ch],    \
          },                                           \
      .pst_q = &u##ch##_q,                             \
      .st_buffer =                                     \
          {                                            \
              .puc_buffer = &auc_u##ch##_buffer[0][0], \
              .puc_index = &uc_u##ch##index,           \
              .us_len = USART##ch##_BUFFER_SZ,         \
              .us_num = USART##ch##_BUFFER_NUM,        \
          },                                           \
      .pst_dma = &st_u##ch##dma,                       \
  }
#define OS_Q list_t
#endif

#ifndef BOOT_PRJ
#define ADD_CHANNEL(ch)                                                        \
  static OS_Q u##ch##_q = {0};                                                 \
  static uint8_t auc_u##ch##_buffer[USART##ch##_BUFFER_NUM]                    \
                                   [USART##ch##_BUFFER_SZ] = {0};              \
  static uint8_t uc_u##ch##index = 0;                                          \
  static st_gpio_t st_u##ch##gpio = {                                          \
      .rx = GPIO_INFO(ch, RX),                                                 \
      .tx = GPIO_INFO(ch, TX),                                                 \
  };                                                                           \
  static const st_dma_parm_t st_u##ch##dma = {                                 \
      .en_dma = USART##ch##_DMAX,                                              \
      .en_dma_ch = USART##ch##_DMA_CH,                                         \
      .en_tr_dir = en_to_mem,                                                  \
      .per_addr = (uint32_t)&USART##ch->DR,                                    \
      .mem_addr =                                                              \
          (uint32_t)&auc_u##ch##_buffer[0][offsetof(__st_packet_t, auc_data)], \
      .us_len = sizeof(auc_u##ch##_buffer[0]),                                 \
      .complete_cb = __ISR,                                                    \
      .complete_cb_arg = (void*)&ast_info[usart##ch],                          \
  };
#else  // defined BOOT_PRJ
#define ADD_CHANNEL(ch)                                                        \
  static OS_Q u##ch##_q = {0};                                                 \
  static uint8_t auc_u##ch##_buffer[USART##ch##_BUFFER_NUM]                    \
                                   [USART##ch##_BUFFER_SZ] = {0};              \
  static uint8_t uc_u##ch##index = 0;                                          \
  static st_gpio_t st_u##ch##gpio = {                                          \
      .rx = GPIO_INFO(ch, RX),                                                 \
      .tx = GPIO_INFO(ch, TX),                                                 \
  };                                                                           \
  static const st_dma_parm_t st_u##ch##dma = {                                 \
      .en_dma = USART##ch##_DMAX,                                              \
      .en_dma_ch = USART##ch##_DMA_CH,                                         \
      .en_tr_dir = en_to_mem,                                                  \
      .per_addr = (uint32_t)&USART##ch->DR,                                    \
      .mem_addr =                                                              \
          (uint32_t)&auc_u##ch##_buffer[0][offsetof(__st_packet_t, auc_data) + \
                                           offsetof(st_q_t, st_pkt)],          \
      .us_len = sizeof(auc_u##ch##_buffer[0]),                                 \
      .complete_cb = __ISR,                                                    \
      .complete_cb_arg = (void*)&ast_info[usart##ch],                          \
  };
#endif  // ifndef BOOT_PRJ

static st_info_t ast_info[];

ADD_CHANNEL(2)  // usart2

static st_info_t ast_info[] = {
    INFO(2),  // usart2
};

static void __init_reg(st_info_t* self) {
  USART_TypeDef* USARTx = self->USARTx;
  uint32_t fck = 0;
  RCC_ClocksTypeDef RCC_Clocks;

  RCC_GetClocksFreq(&RCC_Clocks);
  if (USARTx == USART1)
    fck = RCC_Clocks.PCLK2_Frequency;
  else
    fck = RCC_Clocks.PCLK1_Frequency;

  USARTx->CR1 = CR1_IDLEIE_Set | CR1_TE_Set | CR1_RE_Set | CR1_PEIE_Set;
  USARTx->CR3 = CR3_DMAR_Set | CR3_EIE_Set;
  USARTx->BRR = fck / self->ul_bandrate;
  USARTx->CR1 |= CR1_UE_Set;
  return;
}

static void __init_gpio(st_gpio_t* self) {
  init_gpio(&self->rx);
  init_gpio(&self->tx);
  return;
}

static void inline __init_rcc(st_info_t* self) {
  if (self->USARTx == USART1)
    RCC->APB2ENR |= RCC_APB2ENR(USART1);
  else
    RCC->APB1ENR |= self->ul_rcc_enr;

  return;
}

#ifndef BOOT_PRJ
static void __init_q(const st_q_t* pst_q) {
  OS_ERR err;
  OSQCreate(pst_q->p_q, pst_q->name, pst_q->q_size, &err);
  if (OS_ERR_NONE != err) {
    LOG_ERR("usart q creat failed(%d)!", err);
  }
  return;
}
#else
#define __init_q(pst_q)    \
  do {                     \
    list_init_head(pst_q); \
  } while (0)
#endif

static st_info_t* __init_channel(st_info_t* self) {
  __init_rcc(self);
  __init_reg(self);
  __init_gpio(self->pst_gpio);
#ifndef BOOT_PRJ
  __init_q(&self->st_q);
#else
  __init_q(self->pst_q);
#endif
  start_dma(self->pst_dma);
  reg_int_func(&self->st_int);
  return self;
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
  int ret = -1;
  __st_packet_t* pst_ret = NULL;

#ifndef BOOT_PRJ  // TODO
  OS_ERR err;
  OS_MSG_SIZE msg_size = 0;
  const st_q_t* pst_q = &self->st_q;
  pst_ret = OSQPend(pst_q->p_q, 0, OS_OPT_PEND_BLOCKING, &msg_size, NULL, &err);
  if ((OS_ERR_NONE != err) || (msg_size != sizeof(pst_ret))) {
    LOG_ERR("usart q wait failed(%d)!", err);
    goto Error;
  }
#else
  st_q_t* pst_q = NULL;
  CPU_SR_ALLOC();

  CPU_CRITICAL_ENTER();
  pst_q = (st_q_t*)list_pop_tail(self->pst_q);
  CPU_CRITICAL_EXIT();
  if (NULL == pst_q) goto Error;  // q is empty
  pst_ret = &pst_q->st_pkt;
#endif
  ret = (pst_ret->us_len > sz_len) ? sz_len : pst_ret->us_len;

  memcpy(puc_data, pst_ret->auc_data, ret);
Error:
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

#ifndef BOOT_PRJ
  st_dma.mem_addr =
      (uint32_t)((uint32_t)uc_index * pst_buffer->us_len +
                 offsetof(__st_packet_t, auc_data) + pst_buffer->puc_buffer);
#else
  st_dma.mem_addr =
      (uint32_t)((uint32_t)uc_index * pst_buffer->us_len +
                 offsetof(__st_packet_t, auc_data) + offsetof(st_q_t, st_pkt) +
                 pst_buffer->puc_buffer);
#endif
#if 0  // USART_DBG
  printf("st_dma.mem_addr:0x%x\r\n", st_dma.mem_addr);
#endif
  start_dma(&st_dma);
  *pst_buffer->puc_index = uc_index;
  return;
}

static void post_data(st_info_t* self, uint8_t uc_data) {
  const st_buffer_t* pst_buffer = &self->st_buffer;
  size_t sz_index = (uint32_t)*pst_buffer->puc_index * pst_buffer->us_len;

#ifndef BOOT_PRJ
  __st_packet_t* pst_ret = (__st_packet_t*)&pst_buffer->puc_buffer[sz_index];
  const st_q_t* pst_q = &self->st_q;
  OS_ERR err;

  pst_ret->us_len = get_transterred_size(self->pst_dma);
  OSQPost(pst_q->p_q, pst_ret, sizeof(pst_ret), OS_OPT_POST_FIFO, &err);
  if (OS_ERR_NONE != err) {
    LOG_ERR("usart data post error(%d)!", err);
  }
#else
  st_q_t* pst_ret = (st_q_t*)&pst_buffer->puc_buffer[sz_index];
  CPU_SR_ALLOC();

  pst_ret->st_pkt.us_len = get_transterred_size(self->pst_dma);
  CPU_CRITICAL_ENTER();
  list_add_head(self->pst_q, &pst_ret->p_q);
  CPU_CRITICAL_EXIT();
  // TODO
#endif
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

extern st_drv_if_t* open_usart(en_usart_t channel) {
  assert(channel < sizeof_array(ast_info));
  assert(NULL != &ast_info[channel].USARTx);
  return (st_drv_if_t*)__init_channel(&ast_info[channel]);
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
  st_drv_if_t* usart = open_usart(usart2);

  if (0 == usart->write(usart, "This is a usart test!\r\n",
                        strlen("This is a usart test!\r\n"))) {
    LOG_INFO("usart send test succeed!");
  } else {
    LOG_ERR("usart send test fail!");
  }
  while (1) {
    if (usart->read(usart, auc_tmp, sizeof(auc_tmp)) < 0) continue;
    printf("recv:%s\r\n", auc_tmp);
  }
}
#endif
