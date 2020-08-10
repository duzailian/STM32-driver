#pragma once

typedef enum {
  nrf_ch1,
} en_nrf_t;

extern st_drv_if_t *open_nrf(en_nrf_t channel);

#if NRF_DBG

extern void dbg_nrf(void);
#endif