#pragma once

extern int in_flash_write(uint32_t ul_addr, uint8_t *puc_data, size_t sz_len);
extern int in_flash_erase(uint32_t ul_addr, size_t sz_len);
extern int in_flash_read(uint32_t ul_addr, uint8_t *puc_data, size_t sz_len);
