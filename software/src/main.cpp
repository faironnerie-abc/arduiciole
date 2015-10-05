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

#include "arduiciole.h"

cmd_t message = {CMD_SYNC, 1};

/*
 * @see arduiciole.h#error_mode
 */
void error_mode(uint8_t error) {
  while (1) {
    HELPER_BLINK(CLED, 300, error);
    delay(1000);
  }
}

/*
 * @see arduiciole.h#flash
 */
void flash() {
  unsigned long clock = millis();

  //
  // Dawn
  //

  xbee_transmit();

  for (int i=0; i<=255; i++)
    HELPER_ANALOG_WRITE(i, 1);
  HELPER_DIGITAL_WRITE(HIGH, 0);

  xbee_wait_tx_status();

  clock = millis();

  xbee_transmit();
  xbee_wait_tx_status();

  //
  // Twilight
  //

  xbee_transmit();

  for (int i=255; i>=0; i--)
    HELPER_ANALOG_WRITE(i, 1);
  HELPER_DIGITAL_WRITE(LOW, 0);

  xbee_wait_tx_status();
  delay(LUCIOLE_FLASH_PHASE_LENGTH - min(LUCIOLE_FLASH_PHASE_LENGTH, millis() - clock));
}

/*
 * @see arduiciole.h#sync
 */
void sync() {
  unsigned long clock = millis();
  uint32_t count = 0;
  cmd_t *data;
  uint8_t quit = 0;

  //xbee_flush();

  while (!quit && millis() < clock + LUCIOLE_WAIT_PHASE_LENGTH) {
    data = xbee_receive(max(1, LUCIOLE_WAIT_PHASE_LENGTH - (millis() - clock)));

    if (data != NULL) {
        switch(data->cmd) {
        case CMD_SYNC:
          count++;

          if (count >= LUCIOLE_JUMP_THRESHOLD) { //  && millis() > clock + LUCIOLE_MIN_WAIT_PHASE_LENGTH
            quit = 1;
          }

          break;
        case CMD_RESET:
          delay(random(1000, 5000));
          quit = 1;

          break;
        }
  	}
  }
}

/**
 * Initialisation de l'Arduino.
 *
 * Le port série est ouvert pour permettre la communication avec le module XBee,
 * puis ce dernier est configuré.
 */
void setup() {
  pinMode(CLED, OUTPUT);

  HELPER_BLINK(CLED, 150, 5);

  pinMode(HLED, OUTPUT);
  pinMode(LLED, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(11, OUTPUT);

  //
  // XBee initialization
  //

  xbee_init(&message);
}

/**
 * Boucle principale de l'Arduino.
 */
void loop() {
  xbee_spread();
  
  //
  // Émission de lumière.
  // Bon ok, plus d'électrons que de photons...
  //

  flash();

  //
  // Phase de synchronisation.
  //

  sync();
}
