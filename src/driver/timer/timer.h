#pragma once

typedef enum { timer1, timer2, timer3, timer_max } en_timer_t;

typedef enum {
  timer_ch1,
  timer_ch2,
  timer_ch3,
  timer_ch4,
  timer_ch_max,
} en_timer_ch_t;

typedef struct {
  en_timer_t en_timerx; // timer外设编号
  en_timer_ch_t en_ch; // timer的捕获通道channel
  int_func_t finish_cb; // 采样完成后的DMA中断回调函数
  void *pv_cb_para; // 传入finish_cb函数的指针
  size_t sz_len; // 采样的次数
  void *pv_buffer; // 采样结果缓存buffer
} st_capture_info_t;

extern int timer_init(void);
extern int start_capture(st_capture_info_t *pst_capture_info);

#if TIM_DBG
extern void test_timer(void);
#endif
