#include "include.h"

#define DWT_CR *(CPU_REG32 *)0xE0001000
#define DWT_CYCCNT *(CPU_REG32 *)0xE0001004
#define DEM_CR *(CPU_REG32 *)0xE000EDFC
#define DBGMCU_CR *(CPU_REG32 *)0xE0042004

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR CPU_TS_TmrRd(void)
{
	return ((CPU_TS_TMR)DWT_CYCCNT);
}
#endif