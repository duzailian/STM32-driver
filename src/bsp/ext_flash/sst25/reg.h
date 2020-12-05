#pragma once

#define STATUS_BUSY (1 << 0)
#define STATUS_WEL (1 << 1) // 1 : Device is memory Write enabled
#define STATUS_BP(val) (val << 2)
#define STATUS_AAI (1 << 6) // Auto Address Increment Programming status
#define STATUS_BPL (1 << 7)

#define NO_PROTECT (0)
#define UPPER_1_32_PROTECT (STATUS_BP(1)) // 1F0000H-1FFFFFH
#define UPPER_1_16_PROTECT (STATUS_BP(2)) // 1E0000H-1FFFFFH
#define UPPER_1_8_PROTECT (STATUS_BP(3)) // 1C0000H-1FFFFFH
#define UPPER_1_4_PROTECT (STATUS_BP(4)) // 180000H-1FFFFFH
#define UPPER_1_2_PROTECT (STATUS_BP(5)) // 100000H-1FFFFFH
#define ALL_PROTECT (STATUS_BP(6)) // 000000H-1FFFFFH
