#pragma once


#define init_systick(void)                        \
  {                                               \
    CPU_REG_NVIC_ST_CTRL = 0;                     \
    CPU_REG_NVIC_ST_RELOAD = SysTick_LOAD_RELOAD; \
  }                                               \
  while (0)

#define enable_systick()                                 \
  do {                                                   \
    CPU_REG_NVIC_ST_CTRL |= CPU_REG_NVIC_ST_CTRL_ENABLE; \
  } while (0)

#define disable_systick()                                 \
  do {                                                    \
    CPU_REG_NVIC_ST_CTRL &= ~CPU_REG_NVIC_ST_CTRL_ENABLE; \
  } while (0)

#define get_curr() (SysTick_VAL_CURRENT & CPU_REG_NVIC_ST_CURRENT)
