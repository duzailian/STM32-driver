#pragma once

typedef int (*drv_tr_if_t)(void *self, uint8_t *puc_buffer,
                           size_t len);     // drivers transfer operate function
typedef int (*drv_close_if_t)(void *self);  // drivers close operate function
typedef int (*drv_ioctl)(void *self, int req, ...);
typedef struct {
  drv_tr_if_t write;
  drv_tr_if_t read;
  drv_ioctl ioctl;
  drv_close_if_t close;
} st_drv_if_t;

typedef void (*rx_cb_t)(uint8_t *puc_buffer, size_t sz_len, int channel);
