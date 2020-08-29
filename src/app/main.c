#include "include.h"

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
  OSStart(&err);
  return 0;
}
#endif
