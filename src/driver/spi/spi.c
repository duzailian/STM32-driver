#include "include.h"

/* SPI SPE mask */
#define CR1_SPE_Set ((uint16_t)0x0040)
#define CR1_SPE_Reset ((uint16_t)0xFFBF)

typedef struct {
  gpio_cfg_t sck;
  gpio_cfg_t miso;
  gpio_cfg_t mosi;
} st_gpio_t;

typedef const struct {
  uint32_t ul_bandrate;
  uint8_t uc_phase;  // CPOL & CPHA SPI_CPOL_Low, SPI_CPHA_1Edge etc.
  bool b_lsb_first;  //  MSB/LSB transmitted first
} st_cfg_t;

typedef const struct {
  st_drv_if_t st_drv_if;
  SPI_TypeDef *SPIx;
  uint32_t ul_rcc_enr;
  st_gpio_t st_gpio;
  st_cfg_t st_cfg;
} st_info_t;

#define INFO(ch)                                                  \
  [spi##ch] = {                                                   \
      .st_drv_if =                                                \
          {                                                       \
              .write = __spi_send,                                \
              .read = __spi_recv,                                 \
              .close = __spi_close,                               \
          },                                                      \
      .SPIx = SPI##ch,                                            \
      .ul_rcc_enr = RCC_SPI##ch##_ENR,                            \
      .st_gpio =                                                  \
          {                                                       \
              .sck =                                              \
                  {                                               \
                      .port = GPIO_SPI##ch##_SCK_PORT,           \
                      .pinx = GPIO_SPI##ch##_SCK_PIN,             \
                      .io_mode = gpio_output_10M,                 \
                      .cnf = gpio_af_opp,                         \
                      .af_remap = GPIO_SPI##ch##_REMAP_EN,        \
                      .af_remap_mask = GPIO_SPI##ch##_REMAP_MASK, \
                  },                                              \
              .miso =                                             \
                  {                                               \
                      .port = GPIO_SPI##ch##_MISO_PORT,          \
                      .pinx = GPIO_SPI##ch##_MISO_PIN,            \
                      .io_mode = gpio_input,                      \
                      .cnf = gpio_pull_up,                        \
                      .af_remap = GPIO_SPI##ch##_REMAP_EN,        \
                      .af_remap_mask = GPIO_SPI##ch##_REMAP_MASK, \
                  },                                              \
              .mosi =                                             \
                  {                                               \
                      .port = GPIO_SPI##ch##_MOSI_PORT,          \
                      .pinx = GPIO_SPI##ch##_MOSI_PIN,            \
                      .io_mode = gpio_output_10M,                 \
                      .cnf = gpio_af_opp,                         \
                      .af_remap = GPIO_SPI##ch##_REMAP_EN,        \
                      .af_remap_mask = GPIO_SPI##ch##_REMAP_MASK, \
                  },                                              \
          },                                                      \
      .st_cfg =                                                   \
          {                                                       \
              .ul_bandrate = SPI_BaudRatePrescaler_64,            \
              .uc_phase = SPI_CPOL_Low | SPI_CPHA_1Edge,          \
              .b_lsb_first = false,                               \
          },                                                      \
  }

#define RCC_SPI1_ENR RCC_APB2ENR(SPI1)
#define RCC_SPI2_ENR RCC_APB1ENR(SPI2)
#define RCC_SPI3_ENR RCC_APB1ENR(SPI3)

static int __spi_recv(void *self, uint8_t *puc_buffer, size_t len);
static int __spi_send(void *self, uint8_t *puc_buffer, size_t len);
static int __spi_close(void *self);

static st_info_t ast_info[] = {
    INFO(1),
};

static void inline __init_rcc(st_info_t *self) {
  if (self->SPIx == SPI1)
    RCC->APB2ENR |= RCC_APB2ENR(SPI1);
  else
    RCC->APB1ENR |= self->ul_rcc_enr;

  return;
}

#define __init_gpio(st_info)  \
  do {                        \
    init_gpio(&st_info.sck);  \
    init_gpio(&st_info.miso); \
    init_gpio(&st_info.mosi); \
  } while (0)

static void __init_reg(st_info_t *self) {
  SPI_TypeDef *SPIx = self->SPIx;
  st_cfg_t *pst_cfg = &self->st_cfg;
  uint32_t ul_tmp = 0;

  ul_tmp = SPI_CR1_SSM | SPI_Mode_Master |
           ((uint32_t)pst_cfg->b_lsb_first << 7) | pst_cfg->ul_bandrate |
           pst_cfg->uc_phase;
  SPIx->CR1 = ul_tmp;
  return;
}

static st_drv_if_t *__init_ch(st_info_t *self) {
  __init_rcc(self);
  __init_gpio(self->st_gpio);
  __init_reg(self);
  return (st_drv_if_t *)self;
}

static int __spi_recv(void *__self, uint8_t *puc_buffer, size_t len) {
  st_info_t *self = (st_info_t *)__self;
  SPI_TypeDef *SPIx = self->SPIx;
  size_t i = 0;

  SPIx->DR = puc_buffer[0];
  SPIx->CR1 |= CR1_SPE_Set;
  for (i = 0; i < len; i++) {
    while (!(SPIx->SR & SPI_I2S_FLAG_RXNE))
      ;
    puc_buffer[i] = SPIx->DR;
    while (!(SPIx->SR & SPI_I2S_FLAG_TXE))
      ;
    SPIx->DR = 0;
  }
  SPIx->CR1 &= ~CR1_SPE_Set;

  return i;
}

static int __spi_send(void *__self, uint8_t *puc_buffer, size_t len) {
  st_info_t *self = (st_info_t *)__self;
  SPI_TypeDef *SPIx = self->SPIx;
  size_t i = 0;
  uint32_t tmp = 0;

  SPIx->DR = puc_buffer[i++];
  SPIx->CR1 |= CR1_SPE_Set;
  for (; i < len; i++) {
    while (!(SPIx->SR & SPI_I2S_FLAG_TXE))
      ;
    SPIx->DR = puc_buffer[i];
    while (!(SPIx->SR & SPI_I2S_FLAG_RXNE))
      ;
    tmp = SPIx->DR;
  }
  SPIx->CR1 &= ~CR1_SPE_Set;
  (void)tmp;
  return i;
}

static int __spi_close(void *self) {
  int ret = 0;
  return ret;
}

extern st_drv_if_t *open_spi(en_spi_t channel) {
  assert(channel < sizeof_array(ast_info));
  assert(NULL != ast_info[channel].SPIx);
  return __init_ch(&ast_info[channel]);
}

#if SPI_DBG

static void init_nrf24l01_gpio(void) {
  static gpio_cfg_t gpio = {
      .port = GPIO_NRF1_CS_PORT,
      .pinx = GPIO_NRF1_CS_PIN,
      .io_mode = gpio_output_2M,
      .cnf = gpio_gen_opp,
      .af_remap = 0,
      .af_remap_mask = 0,
  };
  init_gpio(&gpio);
  return;
}

extern void test_spi(void) {
  st_drv_if_t *spi_if = open_spi(spi1);
  static uint8_t auc_tmp[10] = {0x0a};

  init_nrf24l01_gpio();

  reset_pin(gpioe, gpio_pin8);
  spi_if->read(spi_if, auc_tmp, 6);
  set_pin(gpioe, gpio_pin8);

  for (int i = 0; i < 6; i++) printf("0x%x ", auc_tmp[i]);
  printf("\r\n");
}
#endif
