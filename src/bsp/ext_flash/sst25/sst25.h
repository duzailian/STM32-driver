#pragma once

extern void sst25_init(void);
extern int sst25_erase_sector(uint32_t ul_addr);
extern int sst25_write(uint32_t ul_addr, uint8_t *puc_buffer, size_t sz_len);
extern int sst25_read(uint32_t ul_addr, uint8_t *puc_buffer, size_t sz_len);
extern void sst25_protect_cfg(en_ctrl_t en_ctrl);
