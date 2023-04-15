#include "include.h"

#include "version.h"

#ifdef BOOT_PRJ
int main(int arc, char **argv) {
  init_driver();
  init_bsp();
  iap_proc();
  return 0;
}
#else
int main(int argc, char **argv) {
  OS_ERR err;

  init_driver();
  init_os();
  init_bsp();
  printf("firmware version:%s\r\n", FIRMWARE_VERSION);
  OSStart(&err);
  (void)argc;
  (void)argv;
  return 0;
}
#endif


void _exit(int status) {
  (void) status;
  while(1);
}
