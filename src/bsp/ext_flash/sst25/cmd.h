#pragma once

#define WRITE_EN 0x06 // Write Enable
#define AAI_WRITE 0xad // AUTO ADDRESS INCREMENT WORD-PROGRAM
#define V_SR_WRITE_EN 0x50 // Volatile SR Write Enable
#define WRITE_DIS 0x04 // Write Disable(WRDI)
#define READ_STA 0x05 // Read Status Register
#define WRITE_STA_R1 0x01 // Write Status Register-1
#define READ_STA_R2 0x35 // Read Status Register-2
#define WRITE_STA_R2 0x31 // Write Status Register-2
#define READ_STA_R3 0x15 // Read Status Register-3
#define WRITE_STA_R3 0x11 // Write Status Register-3
#define CHIP_ERASE 0xC7 // Chip Erase (or 60h)
#define ERASE_WRITE_SUSPEND 0x75 // Erase / Program Suspend
#define ERASE_WRITE_RESUME 0x7A // Erase / Program Resume
#define PWR_DOWN 0xB9 // Power-down
#define RELEASE_PWR_DOWN 0xAB // Release Power-down / ID
#define DEVICE_ID 0x90 // Manufacturer/Device ID
#define JEDEC_ID 0x9F // JEDEC ID
#define GLOBAL_BLOCK_LOCK 0x7E // Global Block Lock
#define GLOBAL_BLOCK_UNLOCK 0x98 // Global Block Unlock
#define ENTER_QPI_MODE 0x38 // Enter QPI Mode
#define RESET_EN 0x66 // Enable Reset
#define RESET_DEV 0x99 // Reset Device

#define READ_UID 0x4B // Read Unique ID
#define BYTE_WRITE 0x02 // Byte Program
#define QUAD_PAGE_WRITE 0x32 // Quad Page Program
#define SECTOR_ERASE 0x20 // Sector Erase (4KB)
#define BLOCK_ERASE32K 0x52 // Block Erase (32KB)
#define BLOCK_ERASE64K 0xD8 // Block Erase (64KB)
#define READ_DATA 0x03 // Read Data
#define FAST_READ 0x0B // Fast Read
#define FAST_READ_DUAL_OUTPUT 0x3B // Fast Read Dual Output
#define FAST_READ_QUAD_OUTPUT 0x6Bh // Fast Read Quad Output
#define READ_SFDP_REG 0x5Ah // Read SFDP Register
#define ERASE_SECURITY_REG 0x44 // Erase Security Register
#define WRITE_SECURITY_REG 0x42 // Program Security Register
#define READ_SECURITY_REG 0x48 // Read Security Register
#define INDIVIDUAL_BLOCK_LOCK 0x36 // Individual Block Lock
#define INDIVIDUAL_BLOCK_UNLOCK 0x39 // Individual Block Unlock
#define READ_BLOCK_LOCK 0x3D // Read Block Lock
