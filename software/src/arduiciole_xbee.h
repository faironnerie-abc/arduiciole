#ifndef __ARDUCIOLE_XBEE_H__
#define __ARDUCIOLE_XBEE_H__

#include "arduiciole.h"

void xbee_init(cmd_t*);
void xbee_transmit();
cmd_t* xbee_receive(unsigned long);

#endif /* __ARDUCIOLE_XBEE_H__ */
