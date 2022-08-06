#include "include.h"

#if DRV_DBG
void drv_dbg(void) {
#if TIM_DBG
  test_timer();
#endif
#if USART_DBG
  test_usart();
#endif
#if SPI_DBG
  test_spi();
#endif
#if SYS_TICK_DBG
  systick_dbg();
#endif
  return;
}
#endif

extern void init_driver(void) {
#ifndef BOOT_PRJ
  int_dis_all(); // disable alll interrupt
#endif
  init_log();
  init_int();
  timer_init();
#if DRV_DBG && defined(BOOT_PRJ)
  drv_dbg();
#endif
  return;
}
