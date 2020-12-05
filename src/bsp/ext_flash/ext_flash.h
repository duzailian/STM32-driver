#pragma once

#include "cfg.h"

#if SST25_EN
#include "sst25/sst25.h"
#endif

#if SST25_EN
#define ext_flash_write sst25_write
#define ext_flash_read sst25_read
#define ext_flash_erase sst25_erase
#endif

extern void ext_flash_init(void);
