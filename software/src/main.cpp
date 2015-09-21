/**
 * Copyright (c) 2015 Guilhelm Savin. All rights reserved.
 *
 * This file is part of the ARDUICIOLE project.
 *
 * ARDUICIOLE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ARDUICIOLE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ARDUICIOLE.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef ENERGIA
  #include "Energia.h"
#else
  #include "Arduino.h"
#endif

#include <XBee.h>

#define HLED 3
#define LLED 5
#define CLED 13

#define A_WRITE(x, d) { analogWrite(HLED, x);  analogWrite(LLED, x);  delay(d); }
#define D_WRITE(x, d) { digitalWrite(HLED, x); digitalWrite(LLED, x); delay(d); }

#define BLINK(L, D, C) for (int blinki = 0; blinki < C; blinki++) \
  { digitalWrite(L, HIGH); delay(D); digitalWrite(L, LOW); delay(D); }

#ifndef TIMEOUT
# define TIMEOUT 2000
#endif

#define ERROR_NO_XBEE_RESPONSE 3
#define ERROR_BAD_XBEE_RESPONSE 4

typedef struct {
  uint8_t  cmd;
  uint32_t data;
} cmd_t;

#define CMD_SYNC  0x01
#define CMD_RESET 0x02

cmd_t message = {CMD_SYNC, 0};
uint8_t *payload = (uint8_t*) &message;

XBee xbee = XBee();

#ifndef __XBEE_SERIE_1__
XBeeAddress64 addr64 = XBeeAddress64(ZB_BROADCAST_ADDRESS);
ZBTxRequest tx = ZBTxRequest(addr64, payload, sizeof(cmd_t));
ZBRxResponse rx = ZBRxResponse();

uint8_t assocCmd[] = {'A','I'};
AtCommandRequest atRequest = AtCommandRequest(assocCmd);
AtCommandResponse atResponse = AtCommandResponse();
#else
Tx16Request tx = Tx16Request(__xbee_coordinator__, payload, sizeof(uint32_t));
Rx16Response rx = Rx16Response();
#endif

void error_mode(uint8_t error) {
  while (1) {
    BLINK(CLED, 300, error);
    delay(1000);
  }
}

void xbee_magic()
{
  digitalWrite(CLED, LOW);

  //
  // Here starts the magic ;)
  // Well, actually it's just some AT commands to set up the network.
  //

#ifdef __XBEE_SERIE_1__
  char thisByte = 0;

	Serial.print("+++");

	while (thisByte != '\r' && thisByte != '\n') {
		if (Serial.available() > 0)
			thisByte = Serial.read();
	}

  BLINK(CLED, 150, 3);

  Serial.print("ATRE\r");
	Serial.print("ATAP2\r");
	Serial.print("ATCE");
	Serial.print(XBEE_COORDINATOR_ENABLE);
	Serial.print("\r");
	Serial.print("ATMY");
	Serial.print(XBEE_ADDRESS);
	Serial.print("\r");
	Serial.print("ATID");
	Serial.print(XBEE_NETWORK);
	Serial.print("\r");
	Serial.print("ATCH0C\r");
  Serial.print("ATCN\r");

  digitalWrite(CLED, HIGH);
#else
  xbee.send(atRequest);

  BLINK(CLED, 150, 3);

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
#endif
}

void setup()
{
  pinMode(CLED, OUTPUT);

  BLINK(CLED, 150, 5);

  pinMode(HLED, OUTPUT);
  pinMode(LLED, OUTPUT);

  //
  // XBee initialization
  //

  Serial.begin(9600);
  xbee.setSerial(Serial);

  xbee_magic();
}

void flash()
{
  xbee.send(tx);

  //
  // This is a part where we check that the packet has been received.
  // But we do not really want to worry with that...
  //
/*
  if (xbee.readPacket(1000)) {
    if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      if (txStatus.getStatus() == SUCCESS) {
        // ...
      } else {
        // ...
      }
    }
  } else if (xbee.getResponse().isError()) {
    // ...
  }
*/
}

void sync()
{
  unsigned long clock = millis();
  unsigned long sum = 0;
  unsigned int count = 0;
  cmd_t *data;

  while (millis() < clock + TIMEOUT) {
    xbee.readPacket(max(1, TIMEOUT - millis() + clock));

    if (xbee.getResponse().isAvailable()) {
#ifdef __XBEE_SERIE_1__
  		if (xbee.getResponse().getApiId() == RX_16_RESPONSE ||
  		    xbee.getResponse().getApiId() == RX_64_RESPONSE) {
  			if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
  				xbee.getResponse().getRx16Response(rx);
          data = (cmd_t*) rx.getData();
  			}
        else {
  				Rx64Response rx = Rx64Response();
  				xbee.getResponse().getRx64Response(rx);
          data = (cmd_t*) rx.getData();
  			}
#else
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
        xbee.getResponse().getZBRxResponse(rx);
        data = (cmd_t*) rx.getData();
#endif
        //
        // We do not use "1" here, so we let the door open to some improvments
        // where one flash can be done for several devices.
        //

        switch(data->cmd) {
        case CMD_SYNC:
          count += *((uint32_t*)data);
          sum += (millis() - clock) * *((uint32_t*)data);
          break;
        case CMD_RESET:
          break;
        }
  		}
  	}

    sum /= count;
  }

  message.data = sum;

  delay(max(0, clock + TIMEOUT + sum - millis()));
}

void loop()
{
  //
  // Dawn
  //

  for (int i=0; i<=255; i++)
    A_WRITE(i, 1);

  D_WRITE(HIGH, 0);

  //
  // XBee-flash
  //

  flash();
  delay(1000);

  //
  // Twilight
  //

  for (int i=255; i>=0; i--)
    A_WRITE(i, 2);

  D_WRITE(LOW, 0);

  //
  // Finaly, sync
  //

  //sync();
  delay(2000);
}
