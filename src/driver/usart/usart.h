#pragma once

typedef enum {
  usart1,
  usart2,
  usart_max,
} en_usart_t;

extern st_drv_if_t *open_usart(en_usart_t channel);

#if USART_DBG
extern void test_usart(void);
#endif