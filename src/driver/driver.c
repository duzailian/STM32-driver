#include "include.h"

extern void init_driver(void) {
  int_dis_all();  // disable alll interrupt
  init_log();
  init_int();
  return;
}

#if DRV_DBG
extern void drv_dbg(void) {
#if USART_DBG
  test_usart();
#endif
#if SPI_DBG
  test_spi();
#endif
  return;
}
#endif