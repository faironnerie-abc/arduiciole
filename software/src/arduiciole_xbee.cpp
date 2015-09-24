#include "arduiciole_xbee.h"

#include <XBee.h>

uint8_t *payload;

XBee xbee = XBee();

XBeeAddress64 addr64 = XBeeAddress64(ZB_BROADCAST_ADDRESS);
ZBTxRequest tx;
ZBRxResponse rx = ZBRxResponse();

uint8_t assocCmd[] = {'A','I'};
AtCommandRequest atRequest = AtCommandRequest(assocCmd);
AtCommandResponse atResponse = AtCommandResponse();

/**
 * Configuration du module XBee.
 *
 * La LED intégrée s'éteint jusqu'à ce que la configuration réussisse. Dans le
 * cas contraire, la luciole rentre en mode d'erreur.
 */
void xbee_init(cmd_t *cmd) {
  payload = (uint8_t*) cmd;
  tx = ZBTxRequest(addr64, payload, sizeof(cmd_t));

  Serial.begin(9600);
  xbee.setSerial(Serial);

  digitalWrite(CLED, LOW);

  xbee.send(atRequest);

  HELPER_BLINK(CLED, 150, 3);

  if (xbee.readPacket(5000)) {
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(atResponse);

      if (atResponse.isOk()) {
        digitalWrite(CLED, HIGH);
      }
      else {
        error_mode(ERROR_BAD_XBEE_RESPONSE);
      }
    }
    else {
      error_mode(ERROR_BAD_XBEE_RESPONSE);
    }
  }
  else {
    error_mode(ERROR_NO_XBEE_RESPONSE);
  }
}

void xbee_transmit() {
  xbee.send(tx);
}

cmd_t* xbee_receive(unsigned long to) {
  xbee.readPacket(to);

  if (xbee.getResponse().isAvailable()) {
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      xbee.getResponse().getZBRxResponse(rx);
      return (cmd_t*) rx.getData();
    }
  }

  return NULL;
}
