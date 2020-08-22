#include "include.h"
#include "inf.h"

typedef enum {
  en_event,
  en_interrupt,
} en_type_t;

typedef enum {
  en_rising,
  en_falling,
  en_all_edge,
} en_trigger_t;

typedef struct {
  int_func func;   // callback function
  void *func_arg;  // parameter for callback function
} st_cb_t;

typedef struct {
  en_exti_ch_t en_ch;
  en_type_t en_type;
  en_trigger_t en_trigger;
  st_cb_t *p_func;
  en_int_t en_int;  // interrupt channel
} st_info_t;

#define INFO(ch)                     \
  [EXTI_X(ch)] = {                   \
      .en_ch = EXTI_X(ch),           \
      .en_type = EXTI##ch##_TYPE,    \
      .en_trigger = EXTI##ch##_EDGE, \
      .p_func = &__$$##ch##_fuc,     \
      .en_int = EXTI##ch##_IRQ,      \
  }

#define ADD_CHANNEL(ch) static st_cb_t __$$##ch##_fuc;

ADD_CHANNEL(9)
static st_info_t ast_info[] = {
    INFO(9),
};

static int __isr(void *__self) {
  st_info_t *self = (st_info_t *)__self;

  if (EXTI->PR & (1 << self->en_ch)) {
    EXTI->PR = 1 << self->en_ch;
    if (self->p_func) {
      st_cb_t *p_func = self->p_func;
      p_func->func(p_func->func_arg);
    }
  }
  return 0;
}

static inline void __init_int(st_info_t *self, st_exti_t *pst_cfg) {
  st_int_parm_t st_int_parm;
  st_cb_t *p_func = self->p_func;

  assert(NULL != p_func);
  p_func->func = pst_cfg->func;
  p_func->func_arg = pst_cfg->func_arg;
  st_int_parm.en_int = self->en_int;
  st_int_parm.func = __isr;
  st_int_parm.p_arg = (void *)self;
  reg_int_func(&st_int_parm);

  return;
}

static inline void __init_reg(st_info_t *self, st_exti_t *pst_cfg) {
  if (en_interrupt == self->en_type)
    EXTI->IMR |= self->en_ch;
  else
    EXTI->EMR |= self->en_ch;
  if ((en_all_edge == self->en_trigger) || (en_rising == self->en_trigger))
    EXTI->RTSR |= self->en_ch;
  if ((en_all_edge == self->en_trigger) || (en_falling == self->en_trigger))
    EXTI->FTSR |= self->en_ch;
  return;
}

extern int init_exti(st_exti_t *pst_cfg) {
  en_exti_ch_t en_ch = en_exti_0;
  st_info_t *self = NULL;

  assert(NULL != pst_cfg);
  en_ch = pst_cfg->en_ch;
  self = &ast_info[en_ch];

  /*init interrupt*/
  __init_int(self, pst_cfg);
  /*init register*/
  __init_reg(self, pst_cfg);
  return 0;
}
