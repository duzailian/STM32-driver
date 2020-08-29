#include "cfg.h"
#include "include.h"
#include "protocol.h"

#define BUFFER_SZ 255
#define RETRY_CNT 10

typedef void (*jump_func_t)(void);

typedef enum {
#if IAP_MODE & IAP_MODE_USART
  usart_iap,
#endif
#if IAP_MODE & IAP_MODE_SD_CARD
  nrf_iap,
#endif
#if IAP_MODE & IAP_MODE_NRF
  sd_card_iap,
#endif
} en_iap_t;

typedef struct {
  uint8_t auc_buffer[BUFFER_SZ];
  uint8_t uc_len;
} st_packet_t;

typedef const struct {
  open_if_t open_if;
  int channel;
  st_packet_t *pst_packet;
  st_drv_if_t **ppst_drv_if;
} st_info_t;

typedef int (*proc_func)(st_info_t *self, void **p_proc_func);

#define INFO(mode)                        \
  [mode##_iap] = {                        \
      .open_if = (open_if_t)&open_##mode, \
      .channel = IAP_##mode,              \
      .pst_packet = &st_packet,           \
      .ppst_drv_if = &pst_drv_if,         \
  }

static int read_data(st_info_t *self, void **p_proc_func);

static st_packet_t st_packet = {0};
static st_drv_if_t *pst_drv_if = NULL;

static st_info_t ast_info[] = {
#if IAP_MODE & IAP_MODE_USART
    INFO(usart),
#endif
#if IAP_MODE & IAP_MODE_NRF
    INFO(nrf),
#endif
#if IAP_MODE & IAP_MODE_SD_CARD
    INFO(sd_card),
#endif
};

static int __iap_check(void) {
  int ret = -1;
  gpio_cfg_t iap_gpio = {.port = GPIO_IAP_PORT,
                         .pinx = GPIO_IAP_PIN,
                         .io_mode = gpio_input,
                         .cnf = gpio_pull_up,
                         .af_remap = 0};

  init_gpio(&iap_gpio);
  if (0 == get_pin(iap_gpio.port, iap_gpio.pinx)) {
    ret = 0;
  }
  return ret;
}

static void __jump2app(void) {
  jump_func_t JumpToApplication;

  /*check the application address context whether avilible*/
  if (((*(__IO uint32_t *)APPLICATION_ADDRESS) & 0x2FFE0000) == 0x20000000) {
    /* Jump to user application */
    JumpToApplication =
        (jump_func_t)(*(__IO uint32_t *)(APPLICATION_ADDRESS + 4));
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);
    JumpToApplication();
  }
  return;
}
static int init_if(st_info_t *self, void **p_proc_func) {
  st_drv_if_t *pst_drv_if = self->open_if(self->channel);
  int i = 0;
  uint8_t recv_buffer[sizeof(START_CODE)] = {0};

  assert(NULL != pst_drv_if);
  *self->ppst_drv_if = pst_drv_if;  // save driver if
  for (i = 0; i < RETRY_CNT; i++) {
    if ((pst_drv_if->read(pst_drv_if, recv_buffer, sizeof(recv_buffer)) >= 0) &&
        (0 == memcmp(recv_buffer, START_CODE,
                     sizeof(START_CODE)))) {         // received start code
      pst_drv_if->write(pst_drv_if, START_CODE_ACK,  // send ack
                        sizeof(START_CODE_ACK));
      goto Success;
    }
  }
Success:
  *(proc_func *)p_proc_func = read_data;
  return 0;
Error:
  return -1;
}
static int read_data(st_info_t *self, void **p_proc_func) {
  assert(NULL != *self->ppst_drv_if);

Success:
  return 0;
Error:
  return -1;
}

static int unpack_data(st_info_t *self, void **p_proc_func) { return 0; }

static int write_fash(st_info_t *self, void **p_proc_func) { return 0; }

static int __iap_proc(st_info_t *self) {
  proc_func proc_func = init_if;
  int ret = 0;

  while (1) {
    ret = proc_func(self, (void **)&proc_func);
    if (ret < 0) {
      LOG_ERR("IAP error,step:%p", proc_func);
      break;
    }
    if (ret > 0) break;
  }
  return 0;
}

static void __exe_iap(void) {
  int i = 0;

  for (i = 0; i < sizeof_array(ast_info); i++) {
    __iap_proc(&ast_info[i]);
  }
  return;
}

extern void iap_proc(void) {
  if (__iap_check() < 0) {
    __jump2app();
    goto Return;
  }
  __exe_iap();
Return:
  return;
}