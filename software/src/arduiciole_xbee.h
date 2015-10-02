#ifndef __ARDUCIOLE_XBEE_H__
#define __ARDUCIOLE_XBEE_H__

#include "arduiciole.h"

void xbee_init(cmd_t*);
void xbee_transmit();
void xbee_flush();
void xbee_wait_tx_status();
cmd_t* xbee_receive(uint16_t*, unsigned long);

#endif /* __ARDUCIOLE_XBEE_H__ */
