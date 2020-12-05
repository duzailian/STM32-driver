#pragma once

/* Flash Access Control Register bits */
#define ACR_LATENCY_Mask ((uint32_t)0x00000038)
#define ACR_HLFCYA_Mask ((uint32_t)0xFFFFFFF7)
#define ACR_PRFTBE_Mask ((uint32_t)0xFFFFFFEF)

/* Flash Access Control Register bits */
#define ACR_PRFTBS_Mask ((uint32_t)0x00000020)

/* Flash Control Register bits */
#define CR_PG_Set ((uint32_t)0x00000001)
#define CR_PG_Reset ((uint32_t)0x00001FFE)
#define CR_PER_Set ((uint32_t)0x00000002)
#define CR_PER_Reset ((uint32_t)0x00001FFD)
#define CR_MER_Set ((uint32_t)0x00000004)
#define CR_MER_Reset ((uint32_t)0x00001FFB)
#define CR_OPTPG_Set ((uint32_t)0x00000010)
#define CR_OPTPG_Reset ((uint32_t)0x00001FEF)
#define CR_OPTER_Set ((uint32_t)0x00000020)
#define CR_OPTER_Reset ((uint32_t)0x00001FDF)
#define CR_STRT_Set ((uint32_t)0x00000040)
#define CR_LOCK_Set ((uint32_t)0x00000080)

/* FLASH Mask */
#define RDPRT_Mask ((uint32_t)0x00000002)
#define WRP0_Mask ((uint32_t)0x000000FF)
#define WRP1_Mask ((uint32_t)0x0000FF00)
#define WRP2_Mask ((uint32_t)0x00FF0000)
#define WRP3_Mask ((uint32_t)0xFF000000)
#define OB_USER_BFB2 ((uint16_t)0x0008)

/* FLASH Keys */
#define RDP_Key ((uint16_t)0x00A5)
#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)

/* FLASH BANK address */
#define FLASH_BANK1_END_ADDRESS ((uint32_t)0x807FFFF)

/* Delay definition */
#define EraseTimeout ((uint32_t)0x000B0000)
#define ProgramTimeout ((uint32_t)0x00002000)