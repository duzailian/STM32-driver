#pragma once

#define RCC_ENR(bus, per) RCC_##bus##ENR_##per##EN

#define init_rcc(rcc_reg, value) \
  do {                           \
    rcc_reg |= value;            \
  } while (0)
