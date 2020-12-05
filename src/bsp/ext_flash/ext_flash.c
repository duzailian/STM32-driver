#include "include.h"

extern void ext_flash_init(void) {
#if SST25_EN
  sst25_init();
#endif
  return;
}
