#include "include.h"

int main(int argc, char **argv) {
  OS_ERR err;

  init_driver();
  init_os();
  init_bsp();
  OSStart(&err);
  return 0;
}
