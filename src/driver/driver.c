#include "include.h"

void init_driver(void)
{
	init_int();
	init_usart();
#if DBG
#endif
	return;
}
