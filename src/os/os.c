#include "include.h"

void __init_os(void) {
  CPU_INT32U cpu_clk_freq;
  CPU_INT32U cnts;
  CPU_Init();

  cpu_clk_freq = get_clk_freq(); /* Determine SysTick reference freq. */
  cnts = cpu_clk_freq /
         (CPU_INT32U)OS_CFG_TICK_RATE_HZ; /* Determine nbr SysTick increments */
  OS_CPU_SysTickInit(cnts); /* Init uC/OS periodic time src (SysTick). */

  Mem_Init(); /* Initialize Memory Management Module                  */

#if 0 //(APP_CFG_SERIAL_EN == DEF_ENABLED)
	BSP_Ser_Init(
		115200); /* Enable Serial Interface                              */
#endif
  return;
}

void init_os(void) {
  OS_ERR err;

  OSInit(&err);
  __init_os();

  printf("Creating Application Tasks...\r\n");
  create_task(); /* Create Application Tasks                             */
  return;
}