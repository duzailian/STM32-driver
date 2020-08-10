#include "cfg.h"
#include "cmd.h"
#include "include.h"
#include "reg.h"

#if DEFAULT_CH > RF_CH_MASK
#error "default ch is error!"
#endif

typedef const struct {
  gpio_cfg_t ce;
  gpio_cfg_t cs;
  gpio_cfg_t irq;
} st_gpio_t;

typedef struct {
  en_spi_t spi;
  st_drv_if_t *drv_if;
} st_spi_t;

typedef const struct {
  st_drv_if_t st_drv_if;
  st_gpio_t st_gpio;
  st_spi_t *pst_spi;
} st_info_t;

#define INFO(ch)                                        \
  [nrf_ch##ch] = {                                      \
      .st_drv_if =                                      \
          {                                             \
              .write = __nrf_send,                      \
              .read = __nrf_recv,                       \
              .close = __nrf_close,                     \
          },                                            \
      .st_gpio =                                        \
          {                                             \
              .ce =                                     \
                  {                                     \
                      .gpiox = GPIO_NRF##ch##_CE_PORT,  \
                      .pinx = GPIO_NRF##ch##_CE_PIN,    \
                      .io_mode = gpio_output_2M,        \
                      .cnf = gpio_gen_opp,              \
                      .af_remap = 0,                    \
                      .af_remap_mask = 0,               \
                  },                                    \
              .cs =                                     \
                  {                                     \
                      .gpiox = GPIO_NRF##ch##_CS_PORT,  \
                      .pinx = GPIO_NRF##ch##_CS_PIN,    \
                      .io_mode = gpio_output_2M,        \
                      .cnf = gpio_gen_opp,              \
                      .af_remap = 0,                    \
                      .af_remap_mask = 0,               \
                  },                                    \
              .irq =                                    \
                  {                                     \
                      .gpiox = GPIO_NRF##ch##_IRQ_PORT, \
                      .pinx = GPIO_NRF##ch##_IRQ_PIN,   \
                      .io_mode = gpio_input,            \
                      .cnf = gpio_pull_up,              \
                      .af_remap = 0,                    \
                      .af_remap_mask = 0,               \
                  },                                    \
          },                                            \
      .pst_spi = &__$$##ch##_spi,                       \
  }

#define ADD_CHANNEL(ch)              \
  static st_spi_t __$$##ch##_spi = { \
      .spi = NRF##ch##_SPI_PORT,     \
  };

static int __nrf_recv(void *__self, uint8_t *puc_buffer, size_t len);
static int __nrf_send(void *__self, uint8_t *puc_buffer, size_t len);
static int __nrf_close(void *__self);

ADD_CHANNEL(1)
static st_info_t ast_info[] = {
    INFO(1),
};

static void __write_spi(st_info_t *self, void *buffer, size_t len) {
  st_gpio_t *pst_gpio = &self->st_gpio;
  st_drv_if_t *spi_if = self->pst_spi->drv_if;

  assert(NULL != spi_if);
  reset_pin(pst_gpio->cs.gpiox, pst_gpio->cs.pinx);
  spi_if->write(spi_if, buffer, len);
  set_pin(pst_gpio->cs.gpiox, pst_gpio->cs.pinx);
  return;
}

static void __read_spi(st_info_t *self, void *buffer, size_t len) {
  st_gpio_t *pst_gpio = &self->st_gpio;
  st_drv_if_t *spi_if = self->pst_spi->drv_if;

  assert(NULL != spi_if);
  reset_pin(pst_gpio->cs.gpiox, pst_gpio->cs.pinx);
  spi_if->read(spi_if, buffer, len);
  set_pin(pst_gpio->cs.gpiox, pst_gpio->cs.pinx);
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

  assert(NULL !=puc_buffer);
  auc_tmp[i++] = (uc_addr & REG_ADDR_MASK) | W_REGISTER;
  memcpy(&auc_tmp[i], puc_buffer, len);

  __write_spi(self, auc_tmp, len + i);
  return 0;
}

#define __clr_status(self, flg)      \
  do {                               \
    __write_reg(self, STATUS, &flg); \
  } while (0)

/*Count retransmitted packets.*/
static inline uint8_t __get_retr_cnt(st_info_t *__self) {
  uint8_t ret = 0;
  return ret;
}
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

static inline int get_status(st_info_t *self) {
  int tmp = NOP;
  __read_spi(self, &tmp, 1);
  return tmp;
}

static int read_rx_pl(st_info_t *self, uint8_t *puc_buffer, uint8_t len) {
  uint8_t status = get_status(self);
  uint8_t rx_ch = get_rx_p_num(status);

  if ((0 == (RX_DR & status)) || (RX_FIFO_EMPTY == rx_ch))  // rx FIFO empty
    goto Error;
  // FIXME
  __read_reg(self, RX_PW_P0 + rx_ch, puc_buffer, len);

  return 0;
Error:
  return -1;
}

static int __nrf_recv(void *__self, uint8_t *puc_buffer, size_t len) {
  int ret = 0;
  return ret;
}

static int __nrf_send(void *self, uint8_t *puc_buffer, size_t len) {
  int ret = 0;
  return ret;
}

static int __nrf_close(void *self) {
  int ret = 0;
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

  uc_tmp = CRCO | EN_CRC;
  __write_reg(self, CONFIG, &uc_tmp);  // POWER DOWN / PTX / CRC16

  /*Enable auto acknowledgement data pipe 0 - 5*/
  uc_tmp = ENAA_P0 | ENAA_P1 | ENAA_P2 | ENAA_P3 | ENAA_P4 | ENAA_P5;
  __write_reg(self, EN_AA, &uc_tmp);

  /*Enabled RX Addresses pipe 0 - 5*/
  uc_tmp = ERX_P5 | ERX_P4 | ERX_P3 | ERX_P2 | ERX_P1 | ERX_P0;
  __write_reg(self, EN_RXADDR, &uc_tmp);

  /* Setup of Address Widths */
  uc_tmp = AW_3B;
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
  static uint8_t auc_tmp[100];

  assert(NULL != nrf_if);
  __read_reg((st_info_t *)nrf_if, 0x0a, auc_tmp, 5);
  printf("nRF24l01 status:0x%x\r\n", get_status((st_info_t *)nrf_if));
  for (int i = 0; i < 5; i++) {
    printf("0x%x ", auc_tmp[i]);
  }
  printf("\r\n");
}
#endif
