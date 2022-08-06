#include "include.h"

#if SST25_EN
#include "sst25/sst25.c"
#endif

extern void ext_flash_init(void) {
#if SST25_EN
  sst25_init();
#endif
  return;
}
