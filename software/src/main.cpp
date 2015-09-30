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

cmd_t message = {CMD_SYNC, 0};

uint16_t essaim[LUCIOLE_VIEW];
uint8_t essaim_offset = 0;
uint8_t essaim_size = 0;

/*
 * @see arduiciole.h#is_in_essaim
 */
int is_in_essaim(uint16_t addr) {
  for (uint8_t i = 0; i < essaim_offset; i++) {
    if (essaim[i] == addr)
      return 1;
  }

  return 0;
}

/*
 * @see arduiciole.h#add_to_essaim
 */
void add_to_essaim(uint16_t addr) {
  essaim[essaim_offset] = addr;
  essaim_offset = (essaim_offset + 1) % LUCIOLE_VIEW;
  essaim_size = min(LUCIOLE_VIEW, essaim_size + 1);
}

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
  xbee_transmit();
}

/*
 * @see arduiciole.h#sync
 */
void sync() {
  unsigned long clock = millis();
  unsigned int count = 0;
  cmd_t *data;
  uint16_t from;
  uint8_t quit = 0;

  while (!quit && millis() < clock + LUCIOLE_WAIT_PHASE_LENGTH) {
    data = xbee_receive(&from, max(1, millis() - clock - LUCIOLE_WAIT_PHASE_LENGTH));

    if (data != NULL) {
        switch(data->cmd) {
        case CMD_SYNC:
          if (is_in_essaim(from)) {
            count += data->data;
          }
          else if (essaim_size < LUCIOLE_VIEW || random(1000) < LUCIOLE_ADD_IN_SWARM_PROB * 1000) {
            add_to_essaim(from);
            count += data->data;
          }

          if (count > LUCIOLE_JUMP_THRESHOLD) {
            quit = 1;
          }

          break;
        case CMD_RESET:
          essaim_size = 0;
          essaim_offset = 0;

          delay(random(1, 1000));
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

  //
  // XBee initialization
  //

  xbee_init(&message);
}

/**
 * Boucle principale de l'Arduino.
 */
void loop() {
  //
  // Dawn
  //

  for (int i=0; i<=255; i++)
    HELPER_ANALOG_WRITE(i, 1);

  HELPER_DIGITAL_WRITE(HIGH, 0);

  //
  // Émission de lumière.
  // Bon ok, plus d'électrons que de photons...
  //

  flash();

  //
  // La durée totale de la phase "allumée" est définie par LUCIOLE_FLASH_PHASE_LENGTH
  // mais il faut prendre en compte l"allumage des LEDs (255ms) et l'extinction
  // (2 * 255ms).
  //

  delay(LUCIOLE_FLASH_PHASE_LENGTH - 3 * 255);

  //
  // Twilight
  //

  for (int i=255; i>=0; i--)
    HELPER_ANALOG_WRITE(i, 2);

  HELPER_DIGITAL_WRITE(LOW, 0);

  //
  // Phase de synchronisation.
  //

  sync();
}
