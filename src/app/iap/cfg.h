#pragma once

#define IAP_MODE_SD_CARD (1 << 0)
#define IAP_MODE_USART (1 << 1)
#define IAP_MODE_NRF (1 << 2)

#define IAP_MODE (IAP_MODE_NRF | IAP_MODE_USART | IAP_MODE_SD_CARD)