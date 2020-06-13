#ifndef __BSP_RCC_H
#define __BSP_RCC_H

#define RCC_APB1ENR(peripheral) RCC_APB1ENR_##peripheral##EN
#define RCC_APB2ENR(peripheral) RCC_APB2ENR_##peripheral##EN

#define init_rcc(rcc_reg, value) \
  do {                           \
    rcc_reg |= value;            \
  } while (0)

#endif