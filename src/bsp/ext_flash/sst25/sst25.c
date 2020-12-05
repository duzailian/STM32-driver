#include "include.h"
#include "cmd.h"
#include "reg.h"

#undef DBG
#define DBG 1

#define PAGE_SZ (256) // size of page (bytes)
#define WAIT_CNT (100) // max cycle for checking BUSY in status register

static st_gpio_cfg_t st_gpio_cfg = {
    .port = GPIO_SST25_CS_PORT,
    .pinx = GPIO_SST25_CS_PIN,
    .io_mode = gpio_output_10M,
    .cnf = gpio_gen_opp,
};

typedef struct {
  st_drv_if_t *pst_spi_if;
  st_gpio_cfg_t *pst_gpio_cs;
} st_self_t;

#if DBG
static void sst25_dbg(void);
#endif

static st_self_t st_self = {
    .pst_gpio_cs = &st_gpio_cfg,
};

#define send_1byte_cmd(cmd, self)                   \
  do {                                              \
    uint8_t $$tmp = cmd;                            \
    send_cmd(self, &$$tmp, sizeof($$tmp), NULL, 0); \
  } while (0)

#define write_enable(self)          \
  do {                              \
    send_1byte_cmd(WRITE_EN, self); \
  } while (0)
#define write_disable(self)          \
  do {                               \
    send_1byte_cmd(WRITE_DIS, self); \
  } while (0)

static void send_cmd(st_self_t *self, uint8_t *puc_send, size_t sz_send,
                     uint8_t *puc_recv, size_t recv_len) {
  st_gpio_cfg_t *pst_cs = self->pst_gpio_cs;
  st_drv_if_t *pst_spi_if = self->pst_spi_if;

  reset_pin(pst_cs->port, pst_cs->pinx);
  pst_spi_if->write(pst_spi_if, puc_send, sz_send);
  if (recv_len) {
    pst_spi_if->read(pst_spi_if, puc_recv, recv_len);
  }
  set_pin(pst_cs->port, pst_cs->pinx);
  return;
}

static uint8_t read_status(st_self_t *self) {
  uint8_t ret = READ_STA;

  send_cmd(self, &ret, sizeof(ret), &ret, ret);
  return ret;
}

static void write_status_reg(st_self_t *self, uint8_t uc_val) {
  uint8_t auc_tmp[2];

  write_enable(self);

  auc_tmp[0] = WRITE_STA_R1;
  auc_tmp[1] = uc_val;
  send_cmd(self, auc_tmp, sizeof(auc_tmp), NULL, 0);

  return;
}

static int wait_write_finish(st_self_t *self) {
  int cnt = WAIT_CNT;
  uint32_t status = STATUS_BUSY;

  while ((status & STATUS_BUSY) && cnt) {
    cnt--;
    status = read_status(self);
  }

  return cnt ? 0 : -1;
}

static int write_byte(st_self_t *self, uint32_t ul_addr, uint8_t uc_data) {
  int uc_index = 0;
  uint8_t auc_tmp[5];

  auc_tmp[uc_index++] = BYTE_WRITE; // cmd

  auc_tmp[uc_index++] = (ul_addr >> 16) & 0xff; // address
  auc_tmp[uc_index++] = (ul_addr >> 8) & 0xff; // address
  auc_tmp[uc_index++] = (ul_addr >> 0) & 0xff; // address

  auc_tmp[uc_index++] = uc_data; // data
  send_cmd(self, auc_tmp, uc_index, NULL, 0);
  if (wait_write_finish(self) < 0)
    goto Fail;
  return 0;
Fail:
  return -1;
}

#define aai_first_word(self, addr, buffer)                 \
  do {                                                     \
    uint8_t uc_index = 0;                                  \
    uint8_t auc_tmp[6];                                    \
    auc_tmp[uc_index++] = AAI_WRITE; /*cmd*/               \
    auc_tmp[uc_index++] = (addr >> 16) & 0xff; /*address*/ \
    auc_tmp[uc_index++] = (addr >> 8) & 0xff; /*address*/  \
    auc_tmp[uc_index++] = (addr >> 0) & 0xff; /*address*/  \
    auc_tmp[uc_index++] = buffer[0]; /*Byte0*/             \
    auc_tmp[uc_index++] = buffer[1]; /*Byte1*/             \
    send_cmd(self, auc_tmp, uc_index, NULL, 0);            \
  } while (0)

static int aai_remained_word(st_self_t *self, uint8_t *puc_buffer,
                             size_t sz_len) {
  uint8_t auc_tmp[6] = {AAI_WRITE};

  for (int i = 0; i < (sz_len / 2); i++) {
    auc_tmp[1] = puc_buffer[0];
    auc_tmp[2] = puc_buffer[1];
    puc_buffer += 2;
    send_cmd(self, auc_tmp, 3, NULL, 0);
    if (wait_write_finish(self) < 0)
      goto Fail;
  }
  return sz_len / 2 * 2;
Fail:
  return -1;
}

extern int sst25_read(uint32_t ul_addr, uint8_t *puc_buffer, size_t sz_len) {
  st_self_t *self = &st_self;
  uint8_t auc_tmp[5];
  int index = 0;

  // TODO  check the status register
  auc_tmp[index++] = FAST_READ & 0xff;
  auc_tmp[index++] = (ul_addr >> 16) & 0xff;
  auc_tmp[index++] = (ul_addr >> 8) & 0xff;
  auc_tmp[index++] = (ul_addr >> 0) & 0xff;
  auc_tmp[index++] = 0; // dummy Byte

  send_cmd(self, auc_tmp, index, puc_buffer, sz_len);
  return 0;
}

extern int sst25_erase_sector(uint32_t ul_addr) {
  st_self_t *self = &st_self;
  uint8_t auc_tmp[4];
  uint8_t uc_index = 0;

  assert(ul_addr <= 0xfff);

  write_enable(self);

  auc_tmp[uc_index++] = SECTOR_ERASE; // cmd
  memcpy(&auc_tmp[uc_index], &ul_addr, 3); // address
  uc_index += 3;
  send_cmd(self, auc_tmp, uc_index, NULL, 0);

  write_disable(self);
  return 0;
}

extern int sst25_write(uint32_t ul_addr, uint8_t *puc_buffer, size_t sz_len) {
  st_self_t *self = &st_self;
  int ret = -1;

  assert(ul_addr <= 0xfff);
  assert(sz_len != 0);

  write_enable(self);

  if (1 == sz_len) { // just one byte
    if (write_byte(self, ul_addr, puc_buffer[0]) < 0)
      goto Fail;
    goto Return;
  }
  /*write the first word*/
  aai_first_word(self, ul_addr, puc_buffer);
  if (wait_write_finish(self) < 0)
    goto Fail; // wait finish timeout
  puc_buffer += 2;
  sz_len -= 2;
  ul_addr += 2;

  /*write the remained word*/
  ret = aai_remained_word(self, puc_buffer, sz_len);
  if (ret < 0)
    goto Fail; // wait finish timeout
  puc_buffer += ret;
  ul_addr += ret;

  if (sz_len % 2) { // write the last Byte
    write_disable(self);
    write_enable(self);
    if (write_byte(self, ul_addr, puc_buffer[0]) < 0) {
      goto Fail;
    }
  }
Return:
  write_disable(self);

  return 0;
Fail:
  write_disable(self);
  return -1;
}

extern void sst25_init(void) {
  st_self_t *self = &st_self;

  self->pst_spi_if = open_spi(SST25_SPI_PORT);
  init_gpio(self->pst_gpio_cs);
#if DBG
  sst25_dbg();
#endif

  return;
}

extern void sst25_protect_cfg(en_ctrl_t en_ctrl) {
  st_self_t *self = &st_self;

  if (en_enable == en_ctrl) {
    write_status_reg(self, ALL_PROTECT);
  } else {
    write_status_reg(self, NO_PROTECT);
  }

  return;
}
#if DBG

static void get_info(uint8_t *puc_buffer) {
  st_self_t *self = &st_self;
  uint8_t auc_tmp[4] = {0};
  st_drv_if_t *pst_spi_if = self->pst_spi_if;
  st_gpio_cfg_t *pst_cs = self->pst_gpio_cs;

  auc_tmp[0] = 0x90; // cmd
  auc_tmp[1] = 0;
  auc_tmp[2] = 0;
  auc_tmp[3] = 0;

  reset_pin(pst_cs->port, pst_cs->pinx);
  pst_spi_if->write(pst_spi_if, auc_tmp, 4);
  pst_spi_if->read(pst_spi_if, auc_tmp, 3);
  memcpy(puc_buffer, auc_tmp, 3);
  set_pin(pst_cs->port, pst_cs->pinx);

  return;
}

static void sst25_dbg(void) {
  static uint8_t auc_tmp[1001] = {0};
  st_self_t *self = &st_self;
  uint8_t uc_tmp = 0;

  sst25_erase_sector(0x00);
  auc_tmp[0] = read_status(self);
  sst25_protect_cfg(en_disable);
  auc_tmp[1] = read_status(self);
  sst25_read(0, auc_tmp, sizeof(auc_tmp));
  for (int i = 0; i < sizeof(auc_tmp); i++) {
    auc_tmp[i] = i;
  }

  spi_set_baudrate(self->pst_spi_if, spi_br5);
  sst25_write(0, auc_tmp, sizeof(auc_tmp));
  sst25_read(0, auc_tmp, sizeof(auc_tmp));
  
  write_enable(self);
  write_byte(self, 1000, 0x1c);
  write_disable(self);
  sst25_read(1000, &uc_tmp, 1);

  for (int i = 0; i < sizeof(auc_tmp); i++) {
    if (auc_tmp[i] != (i & 0xff)) {
      printf("flash test failed(%d)!\r\n", i);
      goto Return;
    }
  }
  printf("flash test success!\r\n");
  while (1)
    ;
Return:
  return;
}
#endif
