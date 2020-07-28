#ifndef __DRV_H
#define __DRV_H

typedef enum {
	usart1,
	usart2,
} en_usart_t;

extern void init_usart(void);

extern void usart_send(en_usart_t ch, uint8_t *puc_data, size_t sz_len);

#endif