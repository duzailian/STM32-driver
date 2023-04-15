#include "include.h"
#include "reg.h"

#define RDPRT_KEY (0x00A5)
#define KEY1 0x45670123
#define KEY2 0xCDEF89AB

static inline void unlock_flash(void) {
  if (0 == (FLASH->CR & CR_LOCK_Set))
    goto Return;
  FLASH->KEYR = KEY1;
  FLASH->KEYR = KEY2;
  FLASH->CR |= CR_PG_Set;
Return:
  return;
}

static void lock_flash() {
  return;
}

extern int in_flash_write(uint32_t ul_addr, uint8_t *puc_data, size_t sz_len) {
  int ret = -1;

  unlock_flash();
  lock_flash();
  (void)ul_addr;
  (void)puc_data;
  (void)sz_len;
  return ret;
}

extern int in_flash_erase(uint32_t ul_addr, size_t sz_len) {
  int ret = -1;

  unlock_flash();
  lock_flash();
  (void)ul_addr;
  (void)sz_len;
  return ret;
}

extern int in_flash_read(uint32_t ul_addr, uint8_t *puc_data, size_t sz_len) {
  int ret = -1;

  (void)ul_addr;
  (void)puc_data;
  (void)sz_len;
  return ret;
}
