#include "include.h"

typedef const struct {
  uint32_t rcc_mask;
  GPIO_TypeDef *reg;
} st_info_t;

#define GPIO_RCC_ENR(port) RCC_APB2ENR(IOP##port)

#define INFO_INIT(port) \
  { .rcc_mask = GPIO_RCC_ENR(port), .reg = GPIO##port, }

static st_info_t ast_info[] = {
    INFO_INIT(A), INFO_INIT(B), INFO_INIT(C), INFO_INIT(D), INFO_INIT(E),
#if defined(STM32F10X_HD) || defined(STM32F10X_XL)
    INFO_INIT(F), INFO_INIT(G),
#endif
};

extern void init_gpio(gpio_cfg_t *gpio_cfg) {
  gpiox_t gpiox = gpio_cfg->gpiox;
  st_info_t *info = &ast_info[gpiox];
  GPIO_TypeDef *GPIOX = info->reg;
  uint32_t tmp = (gpio_cfg->io_mode << 4) | (gpio_cfg->mode);

  RCC->APB2ENR |= info->rcc_mask;
  if ((gpio_cfg->io_mode == gpio_input) && (gpio_cfg->mode >= gpio_af_opp))
    RCC->APB2ENR |= RCC_APB2ENR(AFIO);
  tmp <<= (gpio_cfg->pinx % 8) * 4;
  if (gpio_cfg->pinx >= gpio_pin8) {
    GPIOX->CRH &= (0xfful << ((gpio_cfg->pinx % 8) * 4));
    GPIOX->CRH |= tmp;
  }
  else
  {
    GPIOX->CRL &= (0xfful << ((gpio_cfg->pinx % 8) * 4));
    GPIOX->CRL |= tmp;
  }

  return;
}