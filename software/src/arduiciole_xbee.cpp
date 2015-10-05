#include "arduiciole_xbee.h"

#include <XBee.h>

#ifdef DEBUG
#include <AltSoftSerial.h>
AltSoftSerial altSoftSerial;
#endif

typedef struct {
  uint8_t cmd;
  uint32_t sh;
  uint32_t sl;
} spread_my_sl_t;

uint8_t *payload;

XBee xbee = XBee();

XBeeAddress64 zb_broadcast_addr64 = XBeeAddress64(ZB_BROADCAST_ADDRESS);
spread_my_sl_t spread_payload = {CMD_SWARM, 0x13A200, 0x0};
ZBTxRequest tx_broadcast;

ZBTxRequest tx;
ZBRxResponse rx = ZBRxResponse();

XBeeAddress64 zb_swarm[LUCIOLE_VIEW];
uint8_t zb_swarm_offset = 0;
uint8_t zb_swarm_size = 0;

uint8_t assocCmd[] = {'S','L'};
AtCommandRequest atRequest = AtCommandRequest(assocCmd);
AtCommandResponse atResponse = AtCommandResponse();

uint16_t tx_status_expected = 0;

unsigned long last_spread = 0;

void xbee_spread() {
  if (last_spread && millis() < last_spread + XBEE_SPREAD_DELAY) {
    return;
  }
  
  xbee.send(tx_broadcast);
  tx_status_expected++;
#ifdef DEBUG
  digitalWrite(11, LOW);
  altSoftSerial.println("diffusion de l'adresse");
#endif
  xbee_wait_tx_status();

  last_spread = millis();
}

int xbee_is_in_swarm(uint64_t addr) {
  for (uint8_t i = 0; i < zb_swarm_offset; i++) {
    if (zb_swarm[i].get() == addr)
      return 1;
  }

  return 0;
}

void xbee_add_to_swarm(uint64_t addr) {
  zb_swarm[zb_swarm_offset].set(addr);
  zb_swarm_offset = (zb_swarm_offset + 1) % LUCIOLE_VIEW;
  zb_swarm_size = min(LUCIOLE_VIEW, zb_swarm_size + 1);

#ifdef DEBUG
  altSoftSerial.print("Nouveau voisin : ");
  altSoftSerial.println(addr, HEX);
#endif
}

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

  for (uint8_t i = 0; i < LUCIOLE_VIEW; i++) {
    zb_swarm[i] = XBeeAddress64();
  }

  tx_broadcast = ZBTxRequest(zb_broadcast_addr64, (uint8_t*) &spread_payload, sizeof(spread_my_sl_t));
  tx_broadcast.setOption(ZB_TX_BROADCAST);

  payload = (uint8_t*) cmd;
  tx = ZBTxRequest(NULL, payload, sizeof(cmd_t));

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
#ifdef DEBUG
        altSoftSerial.print("ATSL ");
        altSoftSerial.println(atResponse.getValueLength());
#endif
        // TODO : Set SL in spread packet
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
  for (uint8_t i = 0; i < zb_swarm_size; i++) {
    tx.setAddress64(zb_swarm[i]);
    xbee.send(tx);
    tx_status_expected++;
  }
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

  while (millis() < clock && tx_status_expected > 0) {
    xbee.readPacket();

    if (xbee.getResponse().isAvailable() &&
      xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      tx_status_expected--;
#ifdef DEBUG
      altSoftSerial.print("Tx Status reçu, restant ");
      altSoftSerial.println(tx_status_expected);
#endif
    }
  }
}

cmd_t* xbee_receive(unsigned long to) {
  xbee.readPacket(to);

  if (xbee.getResponse().isAvailable()) {

#ifdef DEBUG
    digitalWrite(6, HIGH);
#endif

    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      cmd_t *packet = (cmd_t*) rx.getData();

#ifdef DEBUG
      digitalWrite(7, HIGH);
#endif

      xbee.getResponse().getZBRxResponse(rx);

#ifdef DEBUG
      altSoftSerial.print("paquet reçu de : ");
      altSoftSerial.println(*from, HEX);

      delay(50);
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
#endif
      switch(packet->cmd) {
      /*
       * Paquet d'essaimage.
       *
       * Pas besoin de répercuter celui-ci plus haut, mais on le prend en compte
       * pour mettre à jour le voisinage.
       */
      case CMD_SWARM:
#ifdef DEBUG
        altSoftSerial.print("CMD_SWARM reçue de ");
        altSoftSerial.println(addr, HEX);
#endif

        if (!xbee_is_in_swarm(rx.getRemoteAddress64().get())) {
          if (zb_swarm_size < LUCIOLE_VIEW) {
            xbee_add_to_swarm(rx.getRemoteAddress64().get());
          }
          else if (zb_swarm_size < LUCIOLE_VIEW || random(1000) < LUCIOLE_ADD_IN_SWARM_PROB * 1000) {
            xbee_add_to_swarm(rx.getRemoteAddress64().get());
          }
        }

        break;
      /*
       * Remise à zéro du voisinage dans le cas d'une CMD_RESET.
       */
      case CMD_RESET:
        zb_swarm_size = 0;
        zb_swarm_offset = 0;
      default:
        return packet;
      }
    }
    /*
     * Cas où l'on recevrait un confirmation d'envoi.
     */
    else if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      if (tx_status_expected > 0) {
        tx_status_expected--;
      }
#ifdef DEBUG
      altSoftSerial.print("Tx Status reçu, restant ");
      altSoftSerial.println(tx_status_expected);
#endif
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
