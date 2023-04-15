#pragma once

#define MHZ (1000 * 1000)

#include "crc8/crc8.h"

#define pr_info(fmt, ...) do {\
        printf("[info]func:%s,line:%d", __FUNCTION__, __LINE__);\
        printf(fmt, __VA_ARGS__);\
    }while(0)

#define pr_err(fmt, ...) do {\
        printf("[err]func:%s,line:%d", __FUNCTION__, __LINE__);\
        printf(fmt, ##__VA_ARGS__);\
    }while(0)

#define pr_dbg(fmt, ...) do {\
        printf("[dbg]func:%s,line:%d", __FUNCTION__, __LINE__);\
        printf(fmt, __VA_ARGS__);\
    }while(0)

