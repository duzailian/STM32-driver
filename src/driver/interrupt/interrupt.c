#include "include.h"
//

#define INFO(per)                               \
  [en_##per##_int] = {                          \
      .IRQn = per##_IRQn,                       \
      .PreemptPriority = per##_PreemptPriority, \
      .SubPriority = per##_SubPriority,         \
  }

#define ISR(per)                                           \
  void per##_IRQHandler(void) {                            \
    const st_info_t *pst_info = &ast_info[en_##per##_int]; \
    if (pst_info->func) {                                  \
      CPU_SR_ALLOC();                                      \
      CPU_CRITICAL_ENTER();                                \
      pst_info->func(pst_info->func_arg);                  \
      CPU_CRITICAL_EXIT();                                 \
    } else {                                               \
      LOG_ERR("ISR not register!");                        \
    }                                                      \
  }

typedef struct {
  IRQn_Type IRQn;
  int_func func;
  void *func_arg;
  uint32_t PreemptPriority;
  uint32_t SubPriority;
} st_info_t;

static st_info_t ast_info[] = {
    INFO(DMA1_Channel6),
    INFO(USART2),
};

extern void init_int(void) {
  int i = 0;
  uint32_t group = NVIC_GetPriorityGrouping();
  const st_info_t *pst_info = &ast_info[0];
  uint32_t ul_priority = 0;

  for (i = 0; i < sizeof_array(ast_info); i++) {
    ul_priority = NVIC_EncodePriority(group, pst_info->PreemptPriority,
                                      pst_info->SubPriority);
    NVIC_SetPriority(pst_info->IRQn, ul_priority);
    pst_info++;
  }
  return;
}

extern void int_dis_all(void) {
  CPU_IntDis();
  return;
}

extern int reg_int_func(const st_int_parm_t *parameter) {
  int ret = 0;
  st_info_t *pst_info = &ast_info[parameter->en_int];
  pst_info->func = parameter->func;
  pst_info->func_arg = parameter->p_arg;
  NVIC_EnableIRQ(pst_info->IRQn);
  return ret;
}

ISR(DMA1_Channel6)
ISR(USART2)

