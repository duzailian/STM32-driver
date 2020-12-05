#include "include.h"

typedef const struct {
  uint32_t rcc_mask;
  GPIO_TypeDef *GPIOx;
} st_info_t;

#define GPIO_RCC_ENR(port) RCC_ENR(APB2, IOP##port)

#define INFO_INIT(port) \
  { .rcc_mask = GPIO_RCC_ENR(port), .GPIOx = GPIO##port }

static st_info_t ast_info[] = {
    INFO_INIT(A), INFO_INIT(B), INFO_INIT(C), INFO_INIT(D), INFO_INIT(E),
#if defined(STM32F10X_HD) || defined(STM32F10X_XL)
    INFO_INIT(F), INFO_INIT(G),
#endif
};

extern void init_gpio(st_gpio_cfg_t *st_gpio_cfg) {
  gpiox_t port = st_gpio_cfg->port;
  st_info_t *info = &ast_info[port];
  GPIO_TypeDef *GPIOX = info->GPIOx;
  uint32_t tmp = 0;
  uint8_t cnf = st_gpio_cfg->cnf;

  RCC->APB2ENR |= info->rcc_mask;
  if (gpio_input == st_gpio_cfg->io_mode) {
    if (gpio_pull_up == cnf) {
      cnf = gpio_pull_down;
      GPIOX->BSRR = 1 << st_gpio_cfg->pinx; // pull up
    } else if (gpio_pull_down == cnf) {
      GPIOX->BRR = 1 << st_gpio_cfg->pinx; // pull down
    }
  }
  tmp = ((cnf << 2) | (st_gpio_cfg->io_mode)) & 0xff;
  if ((st_gpio_cfg->io_mode > gpio_input) && (cnf >= gpio_af_opp))
    RCC->APB2ENR |= RCC_ENR(APB2, AFIO);
  tmp <<= (st_gpio_cfg->pinx % 8) * 4;
  if (st_gpio_cfg->pinx >= gpio_pin8) {
    GPIOX->CRH &= ~(0x0ful << ((st_gpio_cfg->pinx % 8) * 4));
    GPIOX->CRH |= tmp;
  } else {
    GPIOX->CRL &= ~(0x0ful << ((st_gpio_cfg->pinx % 8) * 4));
    GPIOX->CRL |= tmp;
  }
  if (st_gpio_cfg->af_remap) { // AFIO remap enable
    RCC->APB2ENR |= RCC_ENR(APB2, AFIO);
    if (st_gpio_cfg->af_remap & 0x80000000) { // AFIO_MAPR2
      AFIO->MAPR2 &= ~st_gpio_cfg->af_remap_mask;
      AFIO->MAPR2 |= (st_gpio_cfg->af_remap_mask & st_gpio_cfg->af_remap);
    } else { // AFIO_MAPR
      AFIO->MAPR &= ~st_gpio_cfg->af_remap_mask;
      AFIO->MAPR |= (st_gpio_cfg->af_remap_mask & st_gpio_cfg->af_remap);
    }
  }

  return;
}

extern void set_pin(gpiox_t port, gpio_pin_t pinx) {
  st_info_t *info = &ast_info[port];
  GPIO_TypeDef *GPIOX = info->GPIOx;

  GPIOX->BSRR = 1 << pinx;
  return;
}

extern void reset_pin(gpiox_t port, gpio_pin_t pinx) {
  st_info_t *info = &ast_info[port];
  GPIO_TypeDef *GPIOX = info->GPIOx;

  GPIOX->BRR = 1 << pinx;
  return;
}

extern int get_pin(gpiox_t port, gpio_pin_t pinx) {
  st_info_t *info = &ast_info[port];
  GPIO_TypeDef *GPIOX = info->GPIOx;

  return GPIOX->IDR & 1 << pinx;
}