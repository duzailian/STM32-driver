#include "include.h"

#define DWT_CR *(CPU_REG32 *)0xE0001000
#define DWT_CYCCNT *(CPU_REG32 *)0xE0001004
#define DEM_CR *(CPU_REG32 *)0xE000EDFC
#define DBGMCU_CR *(CPU_REG32 *)0xE0042004

#define DBGMCU_CR_TRACE_IOEN_MASK 0x10
#define DBGMCU_CR_TRACE_MODE_ASYNC 0x00
#define DBGMCU_CR_TRACE_MODE_SYNC_01 0x40
#define DBGMCU_CR_TRACE_MODE_SYNC_02 0x80
#define DBGMCU_CR_TRACE_MODE_SYNC_04 0xC0
#define DBGMCU_CR_TRACE_MODE_MASK 0xC0

#define DEM_CR_TRCENA (1 << 24)

#define DWT_CR_CYCCNTENA (1 << 0)

uint32_t get_clk_freq(void) {
  RCC_ClocksTypeDef rcc_clocks;

  RCC_GetClocksFreq(&rcc_clocks);

  return ((uint32_t)rcc_clocks.HCLK_Frequency);
}

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void CPU_TS_TmrInit(void) {
  CPU_INT32U cpu_clk_freq_hz;

  DEM_CR |= (CPU_INT32U)DEM_CR_TRCENA; /* Enable Cortex-M3's DWT CYCCNT reg. */
  DWT_CYCCNT = (CPU_INT32U)0u;
  DWT_CR |= (CPU_INT32U)DWT_CR_CYCCNTENA;

  cpu_clk_freq_hz = get_clk_freq();
  CPU_TS_TmrFreqSet(cpu_clk_freq_hz);
}
#endif
