
#include "include.h"

static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
static OS_TCB AppTaskStartTCB = {0};

static void AppTaskStart(void *p_arg) {
  OS_ERR err;

#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(
      &err); /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
  CPU_IntDisMeasMaxCurReset();
#endif
  while (DEF_TRUE) { /* Task body, always written as an infinite loop. */
#if DRV_DBG
    drv_dbg();
#endif
    OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);
  }
}

void create_task(void) {
  OS_ERR err;

  OSTaskCreate(
      (OS_TCB *)&AppTaskStartTCB, /* Create the start task */
      (CPU_CHAR *)"App Task Start", (OS_TASK_PTR)AppTaskStart, (void *)0,
      (OS_PRIO)APP_TASK_START_PRIO, (CPU_STK *)&AppTaskStartStk[0],
      (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,
      (CPU_STK_SIZE)APP_TASK_START_STK_SIZE, (OS_MSG_QTY)5u, (OS_TICK)0u,
      (void *)0, (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
      (OS_ERR *)&err);

  return;
}
