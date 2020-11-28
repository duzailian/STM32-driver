#pragma once

// must be equal to (SRAM_BASE | VECT_TAB_OFFSET)
#define APPLICATION_ADDRESS 0x08005000
#define IAP_MODE_SD_CARD (1 << 0)
#define IAP_MODE_USART (1 << 1)
#define IAP_MODE_NRF (1 << 2)

#define IAP_MODE (IAP_MODE_USART)

#define RETRY_CNT 10

#if IAP_DBG

/*interval of search key during init(ms)*/
#define SHAKE_INR 1

/*interval of packet recive(ms)*/
#define PACKET_INR 50

#else

/*interval of search key during shake hands(ms)*/
#define SHAKE_INR 1

/*interval of packet recive(ms)*/
#define PACKET_INR 110

#endif
