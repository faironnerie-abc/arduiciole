#include "arduiciole_xbee.h"

#include <XBee.h>

#ifdef DEBUG
#include <AltSoftSerial.h>
AltSoftSerial altSoftSerial;
#endif

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
#ifdef DEBUG
  altSoftSerial.begin(9600);
#endif

  payload = (uint8_t*) cmd;
  tx = ZBTxRequest(addr64, payload, sizeof(cmd_t));
  tx.setOption(ZB_TX_BROADCAST);

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
#ifdef DEBUG
  digitalWrite(11, HIGH); // DEBUG
#endif
  xbee.send(tx);
#ifdef DEBUG
  digitalWrite(11, LOW);
  altSoftSerial.println("paquet emit");
#endif
}

void xbee_flush() {
  uint8_t i = 0;

  while (i++ < 6) {
    xbee.readPacket();
  }
}

void xbee_wait_tx_status() {
  unsigned long clock = millis() + ZB_TX_STATUS_TIMEOUT;

  while (millis() < clock) {
    xbee.readPacket();

    if (xbee.getResponse().isAvailable() &&
      xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
#ifdef DEBUG
      altSoftSerial.println("diffusion confirmée");
#endif
      return;
    }
  }
}

cmd_t* xbee_receive(uint16_t *from, unsigned long to) {
  xbee.readPacket(to);

  if (xbee.getResponse().isAvailable()) {

#ifdef DEBUG
    digitalWrite(6, HIGH);
#endif

    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {

#ifdef DEBUG
      digitalWrite(7, HIGH);
#endif

      xbee.getResponse().getZBRxResponse(rx);

      from[0] = rx.getRemoteAddress16();

#ifdef DEBUG
      altSoftSerial.print("paquet reçu de : ");
      altSoftSerial.println(*from, HEX);

      delay(50);
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
#endif

      return (cmd_t*) rx.getData();
    }
#ifdef DEBUG
    else {
      altSoftSerial.print("API ID inconnu : ");
      altSoftSerial.println(xbee.getResponse().getApiId(), HEX);
    }

    delay(50);
    digitalWrite(6, LOW);
#endif
  }

  return NULL;
}
