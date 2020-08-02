#pragma once

typedef int (*drv_tr_oper_t)(void *self,
                             uint8_t *puc_buffer,
                             size_t len); // drivers transfer operate function
typedef int (*drv_close_oper_t)(void *self); // drivers close operate function

typedef struct {
    drv_tr_oper_t write;
    drv_tr_oper_t read;
    drv_close_oper_t close;
} st_drv_oper_t;
