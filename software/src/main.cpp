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

#ifdef DEBUG
#include <AltSoftSerial.h>
AltSoftSerial altSoftSerial;
#endif

cmd_t message = CMD_SYNC;
luciole_t state;

/*
 * @see arduiciole.h#error_mode
 */
void error_mode(uint8_t error) {
  while (1) {
    HELPER_BLINK(CLED, 300, error);
    delay(1000);
  }
}

void listen(unsigned long time_out) {
  unsigned long clock = millis(), clock2;
  uint8_t cmd;

  while (millis() < clock + time_out) {
    cmd = xbee_receive(max(1, time_out - (millis() - clock)));

    if (cmd != CMD_NONE) {
        switch(cmd) {
        case CMD_SYNC:
          clock2 = millis();

          state.swarm_cumul += (clock2 - min(clock2, state.start_at + LUCIOLE_ADJUST_ESTIMATE_MEAN_TX_DELAY));
          state.swarm_size  += 1;
          break;
        case CMD_RESET:
          delay(random(1000, 5000));
          break;
        }
  	}
  }
}

/*
 * @see arduiciole.h#flash
 */
void flash() {
  //
  // Dawn
  //

  xbee_transmit();

  for (int i=0; i<=255; i++)
    HELPER_ANALOG_WRITE(i, 1);
  HELPER_DIGITAL_WRITE(HIGH, 0);

  listen(LUCIOLE_FLASH_LENGTH);

  //
  // Twilight
  //

  for (int i=255; i>=0; i--)
    HELPER_ANALOG_WRITE(i, 1);
  HELPER_DIGITAL_WRITE(LOW, 0);
}

/*
 * @see arduiciole.h#sync
 */
void sync() {
  listen (LUCIOLE_WAIT_PHASE_LENGTH);
}

/**
 * Ajustement de l'état de la luciole.
 */
void adjust() {
  if (state.swarm_size) {
    unsigned long mean = state.swarm_cumul / state.swarm_size;
    long d = mean - (LUCIOLE_FLASH_PHASE_LENGTH + LUCIOLE_WAIT_PHASE_LENGTH) / 2;

#ifdef DEBUG
    altSoftSerial.print("ADJUST : ");
    altSoftSerial.print(mean);
    altSoftSerial.print(" > ");
    altSoftSerial.print((LUCIOLE_FLASH_PHASE_LENGTH + LUCIOLE_WAIT_PHASE_LENGTH) / 2);
    altSoftSerial.print(" > ");
    altSoftSerial.println(d);
#endif
    //
    // Atténuation pour augmenter la durée nécessaire à la synchro.
    //

    d = round(LUCIOLE_ADJUST_EPSILON * d);

    //
    // Le délai d'ajustement doit être compris dans
    //        [-LUCIOLE_ADJUST_BASE_DELAY; LUCIOLE_ADJUST_BASE_DELAY].
    // On obtiendra ainsi un délai final entre 0 et 2 * LUCIOLE_ADJUST_BASE_DELAY.
    //

    d = max(-LUCIOLE_ADJUST_BASE_DELAY, d);
    d = min( LUCIOLE_ADJUST_BASE_DELAY, d);

#ifdef DEBUG
    altSoftSerial.print("Délai d'ajustement ");
    altSoftSerial.println(d);
#endif

    delay(LUCIOLE_ADJUST_BASE_DELAY+d);

    state.swarm_cumul = 0;
    state.swarm_size  = 0;
  }
  else {
    delay(LUCIOLE_ADJUST_BASE_DELAY);
  }
}

/**
 * Initialisation de l'Arduino.
 *
 * Le port série est ouvert pour permettre la communication avec le module XBee,
 * puis ce dernier est configuré.
 */
void setup() {
#ifdef DEBUG
  altSoftSerial.begin(9600);
#endif

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
  state.start_at = millis();

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

  //
  // Phase d'ajustement.
  //

  adjust();
}
