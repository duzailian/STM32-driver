#pragma once

struct __st_drv_if_t;

typedef enum {
  en_disable,
  en_enable,
} en_ctrl_t;

/*drivers transfer operate function*/
typedef int (*drv_tr_if_t)(void *self, uint8_t *puc_buffer, size_t len);
/*drivers close operate function*/
typedef int (*drv_close_if_t)(void *self);
/*drivers ioctl operate function*/
typedef int (*drv_ioctl)(void *self, int req, ...);

typedef struct __st_drv_if_t *(*open_if_t)(int channel);

typedef void (*rx_cb_t)(uint8_t *puc_buffer, size_t sz_len, int channel);

typedef struct __st_drv_if_t {
  drv_tr_if_t write;
  drv_tr_if_t read;
  drv_ioctl ioctl;
  drv_close_if_t close;
} st_drv_if_t;
