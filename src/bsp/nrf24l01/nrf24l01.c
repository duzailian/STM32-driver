#include "cmd.h"
#include "include.h"
#include "reg.h"

#if DEFAULT_CH > RF_CH_MASK
#error "default ch is error!"
#endif

typedef enum {
  en_rx_pip1,
  en_rx_pip2,
  en_rx_pip3,
  en_rx_pip4,
  en_rx_pip5,
  en_rx_pip6,
} en_rx_pip_t;

typedef const struct {
  gpio_cfg_t ce;
  gpio_cfg_t cs;
  gpio_cfg_t irq;
} st_gpio_t;

typedef struct {
  en_spi_t spi;
  st_drv_if_t *drv_if;
} st_spi_t;

typedef struct {
#if NODE_TYPE == MASTER_NODE
  // master mode receive data in callback mode
  rx_cb_t rx_cb;
#else
  en_nrf_ack_t en_ack;
#endif
} st_ioctl_t;

typedef const struct {
  st_drv_if_t st_drv_if;
  st_gpio_t st_gpio;
  st_exti_t st_exti;  // information for EXTI
  st_spi_t *pst_spi;
  st_ioctl_t *pst_ioctl;
} st_info_t;

#define INFO(ch)                                         \
  [nrf_ch##ch] = {                                       \
      .st_drv_if =                                       \
          {                                              \
              .write = __nrf_send,                       \
              .read = __nrf_recv,                        \
              .ioctl = __nrf_ctl,                        \
              .close = __nrf_close,                      \
          },                                             \
      .st_gpio =                                         \
          {                                              \
              .ce =                                      \
                  {                                      \
                      .port = GPIO_NRF##ch##_CE_PORT,   \
                      .pinx = GPIO_NRF##ch##_CE_PIN,     \
                      .io_mode = gpio_output_2M,         \
                      .cnf = gpio_gen_opp,               \
                      .af_remap = 0,                     \
                      .af_remap_mask = 0,                \
                  },                                     \
              .cs =                                      \
                  {                                      \
                      .port = GPIO_NRF##ch##_CS_PORT,   \
                      .pinx = GPIO_NRF##ch##_CS_PIN,     \
                      .io_mode = gpio_output_2M,         \
                      .cnf = gpio_gen_opp,               \
                      .af_remap = 0,                     \
                      .af_remap_mask = 0,                \
                  },                                     \
              .irq =                                     \
                  {                                      \
                      .port = GPIO_NRF##ch##_IRQ_PORT,  \
                      .pinx = GPIO_NRF##ch##_IRQ_PIN,    \
                      .io_mode = gpio_input,             \
                      .cnf = gpio_pull_up,               \
                      .af_remap = 0,                     \
                      .af_remap_mask = 0,                \
                  },                                     \
          },                                             \
      .st_exti =                                         \
          {                                              \
              .en_ch = NRF24L01_IRQ_EXTI,                \
              .func = __isr,                             \
              .func_arg = (void *)&ast_info[nrf_ch##ch], \
          },                                             \
      .pst_spi = &__$$##ch##_spi,                        \
      .pst_ioctl = &__$$##ch##_ioctl,                    \
  }

#define ADD_CHANNEL_COMMON(ch)       \
  static st_spi_t __$$##ch##_spi = { \
      .spi = NRF##ch##_SPI_PORT,     \
  };

#if NODE_TYPE == MASTER_NODE
#define ADD_CHANNEL(ch)  \
  ADD_CHANNEL_COMMON(ch) \
  static st_ioctl_t __$$##ch##_ioctl = {0};
#else
#define ADD_CHANNEL(ch)                  \
  ADD_CHANNEL_COMMON(ch)                 \
  static st_ioctl_t __$$##ch##_ioctl = { \
      .en_ack = DEFAULT_ACK,             \
  };
#endif

static int __nrf_recv(void *self, uint8_t *puc_buffer, size_t len);
static int __nrf_send(void *self, uint8_t *puc_buffer, size_t len);
static int __nrf_ctl(void *self, int req, ...);
static int __nrf_close(void *self);
static int __isr(void *__self);
static int __read_rx_pl(st_info_t *self, int *channel, uint8_t **ppuc_buffer);

ADD_CHANNEL(1)
static st_info_t ast_info[] = {
    INFO(1),
};

static void __write_spi(st_info_t *self, void *buffer, size_t len) {
  st_gpio_t *pst_gpio = &self->st_gpio;
  st_drv_if_t *spi_if = self->pst_spi->drv_if;

  assert(NULL != spi_if);
  reset_pin(pst_gpio->cs.port, pst_gpio->cs.pinx);
  spi_if->write(spi_if, buffer, len);
  set_pin(pst_gpio->cs.port, pst_gpio->cs.pinx);
  return;
}

static void __read_spi(st_info_t *self, void *buffer, size_t len) {
  st_gpio_t *pst_gpio = &self->st_gpio;
  st_drv_if_t *spi_if = self->pst_spi->drv_if;

  assert(NULL != spi_if);
  reset_pin(pst_gpio->cs.port, pst_gpio->cs.pinx);
  spi_if->read(spi_if, buffer, len);
  set_pin(pst_gpio->cs.port, pst_gpio->cs.pinx);
  return;
}

static int __read_reg(st_info_t *self, uint8_t uc_addr, uint8_t *puc_buffer,
                      uint8_t __len) {
  uint8_t auc_tmp[MAX_REG_SZ + 1] = {0};
  size_t len = get_reg_size(uc_addr);  // get register size

  assert(NULL != puc_buffer);
  assert(__len >= (len - 1));

  len++;  // add length of STATUS register

  auc_tmp[0] = uc_addr & REG_ADDR_MASK;
  __read_spi(self, &auc_tmp[0], len);
  // TODO save STATUS register value
  memcpy(puc_buffer, &auc_tmp[1], len - 1);
  return len - 1;
}

static int __write_reg(st_info_t *self, uint8_t uc_addr, uint8_t *puc_buffer) {
  uint8_t auc_tmp[MAX_REG_SZ + 1] = {0};
  size_t len = get_reg_size(uc_addr);
  int i = 0;

  assert(NULL != puc_buffer);
  auc_tmp[i++] = (uc_addr & REG_ADDR_MASK) | W_REGISTER;
  memcpy(&auc_tmp[i], puc_buffer, len);

  __write_spi(self, auc_tmp, len + i);
  return 0;
}

#define __clr_status(self, flg)      \
  do {                               \
    __write_reg(self, STATUS, &flg); \
  } while (0)

#if NODE_TYPE == MASTER_NODE  // only master always in the rx mode
/*Received Power Detector.*/
static inline uint8_t __is_channel_busy(st_info_t *self) {
  uint8_t ret = 0;
  __read_reg(self, RPD, &ret, 1);
  return ret & 0x01;
}
#else

/*Count lost packets.*/
static inline uint8_t __get_lost_cnt(st_info_t *self) {
  uint8_t ret = 0;
  __read_reg(self, OBSERVE_TX, &ret, 1);
  return (ret >> 4) & 0x0f;
}

/*Count retransmitted packets.*/
static inline uint8_t __get_retr_cnt(st_info_t *self) {
  uint8_t ret = 0;
  __read_reg(self, OBSERVE_TX, &ret, 1);
  return ret & 0x0f;
}
#endif

/*Receive address data pipe 0-5*/
#define __set_rx_addrs(self, pipe_num, addr)                 \
  do {                                                       \
    __write_reg(self, (uint8_t)RX_ADDR_P0 + pipe_num, addr); \
  } while (0)

/*
Transmit address. Used for a PTX device only.
(LSByte is written first)
Set RX_ADDR_P0 equal to this address to handle
automatic acknowledge if this is a PTX device with
Enhanced ShockBurst? enabled
*/
#define __set_tx_addrs(self, addr)    \
  do {                                \
    __write_reg(self, TX_ADDR, addr); \
  } while (0)

#if 0 // unused in this moment
/*Number of bytes in RX payload in data pipe x(0-5)*/
static inline uint8_t __get_rx_pl_sz(st_info_t *self, uint8_t pip) {
  uint8_t ret = 0;
  __read_reg(self, RX_PW_P0 + pip, &ret, sizeof(ret));
  return ret;
}

/*FIFO Status Register*/
static inline uint8_t __get_fifo_status(st_info_t *self) {
  uint8_t ret = 0;
  __read_reg(self, FIFO_STATUS, &ret, sizeof(ret));
  return ret;
}

#endif

/*
Used for a PTX device
Pulse the rfce high for at least 10?s to Reuse last
transmitted payload. TX payload reuse is active
until W_TX_PAYLOAD or FLUSH TX is executed.
TX_REUSE is set by the SPI command
REUSE_TX_PL, and is reset by the SPI commands
W_TX_PAYLOAD or FLUSH TX
*/
#define __is_tx_reuse(self) (TX_REUSE & __get_fifo_status(self))

/*TX FIFO empty. */
#define __is_tx_empty(self) (TX_EMPTY & __get_fifo_status(self))

/*RX FIFO full. */
#define __is_rx_full(self) (RX_FULL & __get_fifo_status(self))

/*RX FIFO empty. */
#define __is_rx_empty(self) (RX_EMPTY & __get_fifo_status(self))

/*
Enable dynamic payload length data pipe x(0-5)
Requires EN_DPL and ENAA_Px(0-5)
*/
#define __enable_dynamic_pl_x(self, pip) \
  do {                                   \
    uint8_t $$tmp = DPL_Px(pip);         \
    __write_reg(self, DYNPD, &$$tmp);    \
  } while (0)

/*Enables Dynamic Payload Length */
#define __enable_dynamic_pl(self)       \
  do {                                  \
    uint8_t $$tmp = EN_DPL;             \
    __write_reg(self, FEATURE, &$$tmp); \
  } while (0)

/*Enables Payload with ACK*/
#define __enable_ack_pay(self)          \
  do {                                  \
    uint8_t $$tmp = EN_ACK_PAY;         \
    __write_reg(self, FEATURE, &$$tmp); \
  } while (0)

/*Enables NO ACK*/
#define __enable_no_ack(self)           \
  do {                                  \
    uint8_t $$tmp = EN_DYN_ACK;         \
    __write_reg(self, FEATURE, &$$tmp); \
  } while (0)

#define __send_1byte_cmd(self, cmd) \
  do {                              \
    uint8_t tmp = cmd;              \
    __write_spi(self, &tmp, 1);     \
  } while (0)

#define __flush_tx(spi_if)             \
  do {                                 \
    __send_1byte_cmd(spi_if, FLUSH_TX) \
  } while (0)

#define __flush_rx(spi_if)             \
  do {                                 \
    __send_1byte_cmd(spi_if, FLUSH_RX) \
  } while (0)

#define __reuse_tx_pl(spi_if)             \
  do {                                    \
    __send_1byte_cmd(spi_if, REUSE_TX_PL) \
  } while (0)

static inline int __get_status(st_info_t *self) {
  int tmp = NOP;
  __read_spi(self, &tmp, 1);
  return tmp;
}

static inline int __get_rx_len(st_info_t *self) {
  uint8_t ret[2] = {0};

  ret[0] = R_RX_PL_WID;
  __read_spi(self, ret, sizeof ret);
  if (MAX_FIFO_SZ < ret[1]) {
    /*Flush RX FIFO if the read value is larger than 32 bytes.*/
    __send_1byte_cmd(self, FLUSH_RX);
    LOG_INFO("get nRF24L01 RX length error,flush rx buffer!");
    goto Error;
  }
  return ret[1];
Error:
  return -1;
}

#if NODE_TYPE != MASTER_NODE
static int __write_tx_pl(st_info_t *self, uint8_t *puc_buffer, size_t len) {
  uint8_t cmd = W_TX_PAYLOAD;
  uint8_t auc_tmp[len + 1];
  uint8_t uc_index = 0;

  if (nrf_nack == self->pst_ioctl->en_ack) cmd = W_TX_PAYLOAD_NACK;

  auc_tmp[uc_index++] = (cmd & REG_ADDR_MASK) | W_REGISTER;
  memcpy(&auc_tmp[uc_index], puc_buffer, len);

  __write_spi(self, auc_tmp, len + uc_index);

  return 0;
}
#endif

static int __isr(void *__self) {
  st_info_t *self = (st_info_t *)__self;
  uint8_t *puc_buffer = NULL;
  int channel = 0;
  int status = 0;
  int len = 0;

  status = __get_status(self);
#if NODE_TYPE == MASTER_NODE  // master mode ,receive data in callback
  if (status & RX_DR) {     // Data Ready RX FIFO interrupt.
    len = __read_rx_pl(self, &channel, &puc_buffer);
    if (len > 0) {  // read payload success
      self->pst_ioctl->rx_cb(puc_buffer, len, channel);
    }
  }
#endif
  return 0;
}

/*TX FIFO full flag. */
#define __is_tx_full(status) (TX_FULL & status)

/*
Data pipe number for the payload available for
reading from RX_FIFO
000-101: Data Pipe Number
110: Not Used
111: RX FIFO Empty
*/
#define __get_rx_pip_num(status) ((RX_P_NO_MASK & status) >> RX_P_NO_POS)

/*if rx payload available*/
#define __is_rx_invalid(status) (__get_rx_pip_num(status) > en_rx_pip6)

/*Maximum number of TX retransmits interrupt*/
#define __is_max_retrans(status) (status & MAX_RT)

/*
Data Sent TX FIFO interrupt. Asserted when
packet transmitted on TX. If AUTO_ACK is acti-
vated, this bit is set high only when ACK is
received.
Write 1 to clear bit.
*/
#define __is_tx_sent(status) (status & TX_DS)

/*
Data Ready RX FIFO interrupt. Asserted when
new data arrives RX FIFO
Write 1 to clear bit.
*/
#define __is_rx_ready(status) (status & RX_DR)

/*
read RX payload
*/
static int __read_rx_pl(st_info_t *self, int *channel, uint8_t **ppuc_buffer) {
  uint8_t status = __get_status(self);
  static uint8_t auc_tmp[MAX_REG_SZ + 1] = {0};
  uint8_t rx_ch = __get_rx_pip_num(status);
  int len = 0;

  assert(NULL != channel);
  if ((0 == __is_rx_ready(status)) ||  // not ready
      (__is_rx_invalid(status)))       // rx FIFO empty
  {
    LOG_ERR("read RX FIFO error!");
    goto Error;
  }
  // FIXME
  len = __get_rx_len(self);
  if (len < 0) {
    goto Error;
  }
  *channel = rx_ch;
  *ppuc_buffer = &auc_tmp[1];  // auc_tmp[0] is the value of STATUS register
  len++;                       // add length of STATUS register

  auc_tmp[0] = R_RX_PAYLOAD;
  __read_spi(self, &auc_tmp[0], len);
  // TODO save STATUS register value
  return len - 1;
Error:
  return -1;
}

static int __nrf_recv(void *__self, uint8_t *puc_buffer, size_t len) {
#if NODE_TYPE == MASTER_NODE
  assert(0);  // master receive data only in callback mode
  return -1;
#else
  st_info_t *self = (st_info_t *)__self;
  int ret = 0;
  return ret;
#endif
}

static int __nrf_send(void *__self, uint8_t *puc_buffer, size_t len) {
  st_info_t *self = (st_info_t *)__self;
  int ret = -1;

  // TODO
  assert(len <= MAX_FIFO_SZ);
  assert(NULL != puc_buffer);
#if NODE_TYPE == MASTER_NODE
  // TODO
  (void)ret;
  (void)self;
#else
  if (__write_tx_pl(self, puc_buffer, len) < 0) {
    LOG_ERR("write tx FIFO error!");
    goto Error;
  }
#endif

  return ret;
  // Error:
  //  return -1;
}

static void __chg_freq(st_info_t *self) {
  uint8_t current_freq = 0;

  __read_reg(self, RF_CH, &current_freq, sizeof(current_freq));
  current_freq &= RF_CH_MASK;
  current_freq++;  // increase frequency
  current_freq &= RF_CH_MASK;
  __write_reg(self, RF_CH, &current_freq);
  return;
}

static int __nrf_ctl(void *__self, int req, ...) {
  st_info_t *self = (st_info_t *)__self;
  int ret = -1;
  va_list vl;

  va_start(vl, req);
  switch (req) {
#if NODE_TYPE == MASTER_NODE  // master
    case nrf_set_rx_cb: {   //  only master mode receive data in callback mode
      self->pst_ioctl->rx_cb = va_arg(vl, rx_cb_t);
      ret = 0;
      break;
    }
    case nrt_rpd: {
      uint8_t *puc_ret = va_arg(vl, uint8_t *);
      *puc_ret = __is_channel_busy(self);
      ret = 0;
      break;
    }
#else  // slave
    case nrf_set_ack: {// enable/disable ack
      self->pst_ioctl->en_ack = (en_nrf_ack_t)va_arg(vl, int);
      ret = 0;
      break;
    }
    case nrf_get_los_cnt: {  // get lost count
      uint8_t *puc_cnt = va_arg(vl, uint8_t *);
      *puc_cnt = __get_lost_cnt(self);
      ret = 0;
    }
#endif
    case nrf_chg_freq: {
      __chg_freq(self);
      ret = 0;
      break;
    }
    default:
      ret = -1;
      break;
  }
  va_end(vl);

  return ret;
}

static int __nrf_close(void *__self) {
  int ret = -1;
  return ret;
}

#define __init_gpio(st_info) \
  do {                       \
    init_gpio(&st_info.ce);  \
    init_gpio(&st_info.cs);  \
    init_gpio(&st_info.irq); \
  } while (0)

static void __init_reg(st_info_t *self) {
  uint8_t uc_tmp = 0;

#if NODE_TYPE == MASTER_NODE
  uc_tmp = CRCO | EN_CRC | PRIM_RX;  // master is in RX mode
#else
  uc_tmp = CRCO | EN_CRC;
#endif
  __write_reg(self, CONFIG, &uc_tmp);  // POWER DOWN / PTX / CRC16

  /*Enable auto acknowledgement data pipe 0 - 5*/
  uc_tmp = ENAA_P0 | ENAA_P1 | ENAA_P2 | ENAA_P3 | ENAA_P4 | ENAA_P5;
  __write_reg(self, EN_AA, &uc_tmp);

  /*Enabled RX Addresses pipe 0 - 5*/
  uc_tmp = ERX_P5 | ERX_P4 | ERX_P3 | ERX_P2 | ERX_P1 | ERX_P0;
  __write_reg(self, EN_RXADDR, &uc_tmp);

  /* Setup of Address Widths */
  uc_tmp = AW_5B;
  __write_reg(self, SETUP_AW, &uc_tmp);

  /* Setup of Automatic Retransmission */
  uc_tmp = ARD_750 | ARC_3;
  __write_reg(self, SETUP_RETR, &uc_tmp);

  /* Set RF Channel */
  uc_tmp = DEFAULT_CH;
  __write_reg(self, RF_CH, &uc_tmp);

  /*RF Setup Register*/
  uc_tmp = RF_DR_1M | RF_PWR_0DB;
  __write_reg(self, RF_SETUP, &uc_tmp);

  /*Set Receive address data pipe 0*/
  __set_rx_addrs(self, en_rx_pip1, (uint8_t *)"center");

  /*Set Transmit address*/
  __set_tx_addrs(self, "center");

  /*Enable dynamic payload length*/
  __enable_dynamic_pl_x(self, en_rx_pip1);
  __enable_dynamic_pl_x(self, en_rx_pip2);
  __enable_dynamic_pl_x(self, en_rx_pip3);
  __enable_dynamic_pl_x(self, en_rx_pip4);
  __enable_dynamic_pl_x(self, en_rx_pip5);
  __enable_dynamic_pl_x(self, en_rx_pip6);

  /*Enables Dynamic Payload Length*/
  __enable_dynamic_pl(self);
  __enable_ack_pay(self);
  __enable_no_ack(self);
  return;
}

#define __open_spi(pst_spi)                   \
  do {                                        \
    pst_spi->drv_if = open_spi(pst_spi->spi); \
  } while (0)

static st_drv_if_t *__init_ch(st_info_t *self) {
  __init_gpio(self->st_gpio);
  __open_spi(self->pst_spi);
  __init_reg(self);
  init_exti(&self->st_exti);
  return (st_drv_if_t *)self;
}

extern st_drv_if_t *open_nrf(en_nrf_t channel) {
  assert(channel < sizeof_array(ast_info));
  assert(NULL != ast_info[channel].pst_spi);
  return __init_ch(&ast_info[channel]);
}

#if NRF_DBG
extern void dbg_nrf(void) {
  st_drv_if_t *nrf_if = open_nrf(nrf_ch1);
  static uint8_t auc_tmp[100] = "";

  assert(NULL != nrf_if);
  __set_rx_addrs((st_info_t *)nrf_if, 0, "duzailianabcdefg");
  __read_reg((st_info_t *)nrf_if, RX_ADDR_P0, auc_tmp, sizeof(auc_tmp));
  __enable_dynamic_pl((st_info_t *)nrf_if);
  // printf("nRF24l01 status:0x%x\r\n", __get_status((st_info_t *)nrf_if));
#if 0
  for (int i = 0; i < 5; i++) {
    printf("0x%x ", auc_tmp[i]);
  }
#endif
  printf("%s\r\n", auc_tmp);
}
#endif
