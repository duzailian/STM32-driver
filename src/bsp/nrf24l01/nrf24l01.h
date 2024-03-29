#pragma once

#include "cfg.h"

typedef enum {
  nrf_ch1,
} en_nrf_t;

#if NODE_TYPE != SERVER_MODE
typedef enum {
  en_nrf_nack,
  en_nrf_ack,
} en_nrf_ack_t;
#endif

typedef enum {
#if NODE_TYPE == SERVER_MODE
  nrf_set_rx_cb,
  nrf_rpd, // Received Power Detector
#else
  nrf_set_ack, // set enable/disable ack
  nrf_get_los_cnt, // get lost count
#endif
  nrf_chg_freq,
} en_nrf_ctl_t;

extern st_drv_if_t *open_nrf(en_nrf_t channel);

#if NRF_DBG

extern void dbg_nrf(void);
#endif