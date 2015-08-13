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

XBee xbee;

void xbee_magic()
{
  char thisByte = 0;

  Serial.begin(9600);
  xbee.setSerial(Serial);
	Serial.print("+++");

	while (thisByte != '\r' && thisByte != '\n') {
		if (Serial.available() > 0)
			thisByte = Serial.read();
	}

  //
  // Here starts the magic ;)
  // Well, actually it's just some AT commands to set up the network.
  //

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

  digitalWrite(CLED, LOW);

  xbee = XBee();
  xbee_magic();

  digitalWrite(CLED, HIGH);
}

void loop()
{
  for (int i=0; i<=255; i++)
    A_WRITE(i, 1);

  D_WRITE(HIGH, 1000);

  for (int i=255; i>=0; i--)
    A_WRITE(i, 2);

  D_WRITE(LOW, 3000);
}