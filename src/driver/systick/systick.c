#include "include.h"

#include "reg.h"

extern void delay_ms(uint32_t ms) {
  RCC_ClocksTypeDef RCC_Clocks = {0};
  size_t i = 0;

  RCC_GetClocksFreq(&RCC_Clocks);
  assert((RCC_Clocks.HCLK_Frequency / 8 / 1000) <= SysTick_LOAD_RELOAD);
  CPU_REG_NVIC_ST_RELOAD = RCC_Clocks.HCLK_Frequency / 8 / 1000;
  enable_systick();
  CPU_REG_NVIC_ST_CURRENT = RCC_Clocks.HCLK_Frequency / 8 / 1000;
  for (i = 0; i < ms; i++) {
    while ((CPU_REG_NVIC_ST_CTRL & CPU_REG_NVIC_ST_CTRL_COUNTFLAG) == 0)
      ;
  }
  disable_systick();
  return;
}

#if SYS_TICK_DBG
extern void systick_dbg(void) {
  open_usart(PRINT_USART);
  while (1) {
    delay_ms(100);
    printf("emmm!\r\n");
  }
}
#endif
