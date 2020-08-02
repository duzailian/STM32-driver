#pragma once

typedef int (*int_func)(void *);

typedef enum {
  USART2_DMA_IRQ,
  USART2_IRQ,
} en_int_t;

typedef struct {
  en_int_t en_int;
  int_func func;
  void *p_arg;
} st_int_parm_t;

extern void init_int(void);
extern void int_dis_all(void);
extern int reg_int_func(const st_int_parm_t *parameter);

