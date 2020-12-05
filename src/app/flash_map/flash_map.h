#pragma once

/*Embedded FLASH base address*/
#define IN_FLASH_BASE_ADDR (0x08000000)

#ifdef STM32F10X_CL
#define IN_CODE_FLASH_SZ (0x3FFFF)
#endif

#pragma pack(1)

typedef struct { //  Embedded FLASH
  uint8_t reserved0[IN_FLASH_BASE_ADDR];
  uint8_t auc_code[IN_CODE_FLASH_SZ];
} st_in_flash_t;

typedef struct { //  EXTERNAL FLASH(w25q64 etc.)
  uint8_t reserved0;
  uint8_t auc_code[IN_CODE_FLASH_SZ];
} st_ext_flash_t;
#pragma pack()

#define common_oper(type, oper, member, buffer, len)                    \
  (len = (len > sizeof(((st_##type##_flash_t *)0)->member))             \
             ? sizeof(((st_##type##_flash_t *)0)->member)               \
             : len),                                                    \
      type##_flash_##oper((uint32_t)((st_##type##_flash_t *)0)->member, \
                          buffer, len)

/*internal flash operate*/
#define read_in_flash(member, buffer, len) \
  common_oper(in, read, member, buffer, len)

#define write_in_flash(member, buffer, len) \
  common_oper(in, write, member, buffer, len)

#define erase_in_flash(member, buffer, len) \
  common_oper(in, erase, member, buffer, len)

/*external flash operate*/
#define read_ext_flash(member, buffer, len) \
  common_oper(ext, read, member, buffer, len)

#define write_ext_flash(member, buffer, len) \
  common_oper(ext, write, member, buffer, len)

#define erase_ext_flash(member, buffer, len) \
  common_oper(ext, erase, member, buffer, len)
