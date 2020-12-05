#include "include.h"

/*header files of this module*/
#include "cfg.h"
#include "protocol.h"

/*predeclaration*/
struct __st_self_t;
struct __st_proc_t;
enum __en_err_t;

typedef void (*jump_func_t)(void);
typedef enum __en_err_t (*proc_func_t)(const struct __st_self_t *self,
                                       struct __st_proc_t *pst_info);

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

typedef enum __en_err_t {
  err_none,
  err_len,
  err_prot, // protocol error
  err_crc,
  err_timeout,
  err_finish,
} en_err_t;

typedef struct {
  uint8_t
      auc_buffer[sizeof(st_pkt_head_t) + PACKET_MAX_SZ + sizeof(st_pkt_tail_t)];
  st_drv_if_t *pst_drv_if;
} st_packet_t;

typedef const struct __st_self_t {
  open_if_t open_if;
  int channel;
  st_packet_t *pst_packet;
} st_self_t;

typedef struct __st_proc_t {
  proc_func_t proc_func;
  uint8_t *puc_recv;
  uint16_t us_rxlen;
  uint16_t us_index;
} st_proc_t;

#define INFO(mode)                        \
  [mode##_iap] = {                        \
      .open_if = (open_if_t)&open_##mode, \
      .channel = IAP_##mode,              \
      .pst_packet = &st_packet,           \
  }

static en_err_t status_read_data(st_self_t *self, st_proc_t *pst_proc);
static en_err_t status_parse_data(st_self_t *self, st_proc_t *pst_proc);
static en_err_t status_write_flash(st_self_t *self, st_proc_t *pst_proc);

static st_packet_t st_packet = {0};

static st_self_t ast_self[] = {
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

#ifndef IAP_DBG
static int __is_enable(void) {
  int ret = -1;
  st_gpio_cfg_t iap_gpio = {.port = GPIO_IAP_PORT,
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
#endif

static en_err_t status_read_data(st_self_t *self, st_proc_t *pst_proc) {
  st_packet_t *pst_packet = self->pst_packet;
  st_drv_if_t *pst_drv_if = pst_packet->pst_drv_if;
  int i = 0;
  int len = 0;

  assert(NULL != pst_drv_if);
  for (i = 0; i < RETRY_CNT; i++) {
    len = pst_drv_if->read(pst_drv_if, pst_packet->auc_buffer,
                           sizeof(pst_packet->auc_buffer));
    if (len > 0) {
      /*received packet*/
      goto Success;
    }
    delay_ms(PACKET_INR);
  }

  return err_timeout;
Success:
  pst_proc->puc_recv = &pst_packet->auc_buffer[0];
  pst_proc->us_rxlen = len;
  pst_proc->proc_func = status_parse_data;
  return err_none;
}

static en_err_t status_parse_data(st_self_t *self, st_proc_t *pst_proc) {
  st_pkt_head_t *pst_head = (st_pkt_head_t *)pst_proc->puc_recv;
  st_pkt_tail_t *pst_tail = NULL;
  en_err_t err = err_none;

  if (pst_proc->us_rxlen !=
      (pst_head->us_len + sizeof(st_pkt_head_t) + sizeof(st_pkt_tail_t))) {
    err = err_len;
    goto Fail;
  }

  if (START_CODE != pst_head->uc_start) {
    err = err_prot;
    goto Fail;
  }

  if (pst_head->us_len > PACKET_MAX_SZ) {
    err = err_len;
    goto Fail;
  }
  pst_tail = (st_pkt_tail_t *)(&pst_head->auc_data[0] + pst_head->us_len);
  if (crc_8(&pst_head->auc_data[0], pst_head->us_len) != pst_tail->uc_crc) {
    err = err_crc;
    goto Fail;
  }
  pst_proc->puc_recv = &pst_head->auc_data[0];
  pst_proc->us_rxlen = pst_head->us_len;
  pst_proc->us_index = pst_head->us_index;
  pst_proc->proc_func = status_write_flash;
  return err;
Fail:
  pst_proc->proc_func = status_read_data;
  return err;
}

static en_err_t status_write_flash(st_self_t *self, st_proc_t *pst_proc) {
  pst_proc->proc_func = status_read_data;

  write_ext_flash(auc_code[pst_proc->us_index * PACKET_MAX_SZ],
                  pst_proc->puc_recv, pst_proc->us_rxlen);
#if IAP_DBG
  static uint16_t us_cnt = 0;
  us_cnt++;
#endif
  return err_none;
}

#if IAP_DBG
static void print_err(en_err_t err) {
  const char *err_str[] = {[err_len] = "length",
                           [err_crc] = "CRC",
                           [err_timeout] = "wait data timeout"};
  if (err_none == err)
    goto Return;
  LOG_ERR("IAP error:%s", err_str[err]);
Return:
  return;
}
#endif

static int __iap_proc(st_self_t *self) {
  st_proc_t st_proc = {.proc_func = status_read_data};
  en_err_t ret = err_none;

  while (1) {
    ret = st_proc.proc_func(self, &st_proc);
#if IAP_DBG
    if (err_none != ret)
      print_err(ret);
#endif
    if (err_finish == ret) {
      goto Success;
    }
  }
Success:
  return 0;
}

static en_err_t shake_hands(st_self_t *self) {
  st_drv_if_t *pst_drv_if = self->open_if(self->channel);
  st_packet_t *pst_packet = self->pst_packet;
  int len = 0;
  int i = 0;

  assert(NULL != pst_drv_if);
  pst_packet->pst_drv_if = pst_drv_if; // save driver if
  pst_drv_if->write(pst_drv_if, REQ_CODE, strlen(REQ_CODE));
  for (i = 0; i < RETRY_CNT; i++) {
    len =
        pst_drv_if->read(pst_drv_if, pst_packet->auc_buffer, strlen(START_CMD));
    if ((len >= strlen(START_CMD)) &&
        (0 == strcmp((char *)pst_packet->auc_buffer, START_CMD))) {
      /*received start code, send ack*/
      pst_drv_if->write(pst_drv_if, START_CMD_ACK, strlen(START_CMD_ACK));
      goto Success;
    }
    delay_ms(SHAKE_INR);
  }
  return err_timeout;
Success:
  return err_none;
}

static st_self_t *search_if(void) {
  st_self_t *pst_ret = NULL;

  while (1) {
    for (int i = 0; i < sizeof_array(ast_self); i++) {
      pst_ret = &ast_self[i];
      if (shake_hands(pst_ret) == err_none)
        goto Success;
    }
  }
Success:
  return pst_ret;
}

static void __exe_iap(void) {
  st_self_t *pst_self = NULL;

  pst_self = search_if();
  __iap_proc(pst_self);
  return;
}

#ifdef IAP_DBG
extern void iap_proc(void) {
  __exe_iap();
  return;
}
#else
extern void iap_proc(void) {
  if (__is_enable() < 0) {
    __jump2app(); // IAP disabled
    goto Return;
  }
  __exe_iap();
  NVIC_SystemReset();
Return:
  return;
}
#endif
