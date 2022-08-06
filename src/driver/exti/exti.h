#pragma once

#define EXTI_X(ch) en_exti_##ch
typedef enum {
  EXTI_X(0),
  EXTI_X(1),
  EXTI_X(2),
  EXTI_X(3),
  EXTI_X(4),
  EXTI_X(5),
  EXTI_X(6),
  EXTI_X(7),
  EXTI_X(8),
  EXTI_X(9),
  EXTI_X(10),
  EXTI_X(11),
  EXTI_X(12),
  EXTI_X(13),
  EXTI_X(14),
  EXTI_X(15),
  EXTI_X(16),
  EXTI_X(17),
  EXTI_X(18),
  EXTI_X(19),
} en_exti_ch_t;

typedef const struct {
  en_exti_ch_t en_ch;
  int_func_t func; // callback function
  void *func_arg; // parameter for callback function
} st_exti_t;

extern int init_exti(st_exti_t *pst_cfg);
