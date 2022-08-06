#include "include.h"
//

#define INFO(per)                               \
  [en_##per##_int] = {                          \
      .IRQn = per##_IRQn,                       \
      .PreemptPriority = per##_PreemptPriority, \
      .SubPriority = per##_SubPriority,         \
  }
#ifdef BOOT_PRJ
#define ISR(per)                                       \
  void per##_IRQHandler(void) {                        \
    const st_info_t *self = &ast_info[en_##per##_int]; \
    if (self->func) {                                  \
      self->func(self->func_arg);                      \
    } else {                                           \
      LOG_ERR("ISR not register!");                    \
    }                                                  \
  }
#else
#define ISR(per)                                       \
  void per##_IRQHandler(void) {                        \
    const st_info_t *self = &ast_info[en_##per##_int]; \
    if (self->func) {                                  \
      CPU_SR_ALLOC();                                  \
      CPU_CRITICAL_ENTER();                            \
      self->func(self->func_arg);                      \
      CPU_CRITICAL_EXIT();                             \
    } else {                                           \
      LOG_ERR("ISR not register!");                    \
    }                                                  \
  }
#endif

typedef struct {
  IRQn_Type IRQn;
  int_func_t func;
  void *func_arg;
  uint32_t PreemptPriority;
  uint32_t SubPriority;
} st_info_t;

static st_info_t ast_info[] = {
    INFO(DMA1_Channel6),
    INFO(USART2),
    INFO(EXTI9_5),
};

extern void init_int(void) {
  int i = 0;
  uint32_t group = NVIC_GetPriorityGrouping();
  const st_info_t *self = &ast_info[0];
  uint32_t ul_priority = 0;

  for (i = 0; i < sizeof_array(ast_info); i++) {
    ul_priority =
        NVIC_EncodePriority(group, self->PreemptPriority, self->SubPriority);
    NVIC_SetPriority(self->IRQn, ul_priority);
    self++;
  }
  return;
}

extern void int_dis_all(void) {
  CPU_IntDis();
  return;
}

extern int reg_int_func(const st_int_parm_t *parameter) {
  int ret = 0;
  st_info_t *self = &ast_info[parameter->en_int];
  self->func = parameter->func;
  self->func_arg = parameter->p_arg;
  NVIC_EnableIRQ(self->IRQn);
  return ret;
}

ISR(DMA1_Channel6)

ISR(USART2)

ISR(EXTI9_5)
