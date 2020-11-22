#include "include.h"

void init_bsp(void) {
  return;
}

#if BSP_DBG
extern void bsp_dbg(void) {
#if NRF_DBG
  dbg_nrf();
#endif
  return;
}
#endif