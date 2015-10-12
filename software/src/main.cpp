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

unsigned long cycle_length = LUCIOLE_FLASH_PHASE_LENGTH + LUCIOLE_WAIT_PHASE_LENGTH + LUCIOLE_ADJUST_BASE_DELAY;

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
 * @see arduiciole.h#listen()
 */
void listen(unsigned long time_out) {
  unsigned long clock = millis(), clock2;
  uint8_t cmd;
  uint8_t *data;

  while (millis() < clock + time_out) {
    cmd = xbee_receive(max(1, time_out - (millis() - clock)), &data);

    if (cmd != CMD_NONE) {
        switch(cmd) {
        case CMD_SYNC:
          /*
           * On cherche à déterminer le décalage entre le début du cycle de cette
           * luciole, et le début de celle dont on vient de recevoir le SYNC.
           * Pour prendre en compte la latence liée au réseau, on soustrait
           * la valeur LUCIOLE_ADJUST_ESTIMATE_MEAN_TX_DELAY au délai déterminé.
           * Cependant, si le paquet est réçu alors que la luciole est à une
           * distance temporelle inférieure à LUCIOLE_ADJUST_ESTIMATE_MEAN_TX_DELAY,
           * il faut considérer que le paquet a été envoyé à la fin du cycle précédent
           * et adapter notre calcul en conséquence relativement à `cycle_length`.
           */
          clock2 = millis() - state.start_at;
          clock2 = clock2 + cycle_length - LUCIOLE_ADJUST_ESTIMATE_MEAN_TX_DELAY;
          clock2 = clock2 % cycle_length;

#ifdef DEBUG
          altSoftSerial.print("[CMD] SYNC @ ");
          altSoftSerial.print(millis() - state.start_at);
          altSoftSerial.print(" | REMOTE CLOCK ");
          altSoftSerial.println(clock2);
#endif
          state.swarm_cumul += clock2;
          state.swarm_size  += 1;

          break;
        case CMD_RESET:
          delay(random(LUCIOLE_RESET_MIN_DELAY, LUCIOLE_RESET_MAX_DELAY));

          DEBUG_PRINT("[CMD] RESET")

          break;
        case CMD_EPSILON:
          state.epsilon = data[1] / 255.0;

          DEBUG_PRINT_VAL("[SET] EPSILON = ", state.epsilon)

          break;
        case CMD_ON:
          state.enabled = LUCIOLE_ON;

          DEBUG_PRINT("[SET] ENABLED ON")

          break;
        case CMD_OFF:
          state.enabled = LUCIOLE_OFF;

          DEBUG_PRINT("[SET] ENABLED OFF")

          break;
        case CMD_TX_STATUS:
          DEBUG_PRINT_VAL("[CMD] TX STATUS @ ", millis() - state.start_at)

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

  listen(LUCIOLE_FLASH_LENGTH - min(LUCIOLE_FLASH_LENGTH, millis() - state.start_at + 255));

  //
  // Twilight
  //

  for (int i=255; i>=0; i--)
    HELPER_ANALOG_WRITE(i, 1);
  HELPER_DIGITAL_WRITE(LOW, 0);
}

/*
 * @see arduiciole.h#sync()
 */
void sync() {
  //
  // Le cycle de la luciole, sans la période d'ajustement, doit
  // durer LUCIOLE_FLASH_PHASE_LENGTH + LUCIOLE_WAIT_PHASE_LENGTH ms.
  // On attend donc le temps restant, en lisant les paquets.
  //

  long until = state.start_at + LUCIOLE_FLASH_PHASE_LENGTH + LUCIOLE_WAIT_PHASE_LENGTH;
  until -= millis();

  listen (max(1, until));
}

/*
 * @see arduiciole.h#adjust()
 */
void adjust() {
  unsigned long adjust_delay = LUCIOLE_ADJUST_BASE_DELAY;

  if (state.swarm_size) {
    unsigned long mean = state.swarm_cumul / state.swarm_size;
    long d;

    if (mean < cycle_length / 2) {
      d = mean;
    }
    else {
      d = cycle_length / 2 - mean;
    }

    DEBUG_PRINT_VAL("[ADJ] mean = ", mean)
    DEBUG_PRINT_VAL("      half = ", cycle_length / 2)
    DEBUG_PRINT_VAL("      d    = ", d)
    
    //
    // Atténuation pour augmenter la durée nécessaire à la synchro.
    //

    d = round(state.epsilon * d);

    d = max(-LUCIOLE_ADJUST_BASE_DELAY, d);
    d = min( LUCIOLE_ADJUST_BASE_DELAY, d);

    DEBUG_PRINT_VAL("[ADJ] Délai d'ajustement ", d)

    adjust_delay += d;
  }

  state.swarm_cumul = 0;
  state.swarm_size  = 0;

  listen(adjust_delay);
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

#ifdef DEBUG
  //
  // LEDs de débogage.
  //

  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(11, OUTPUT);
#endif

  //
  // Initialisation de l'état de la luciole
  //

  state.start_at = 0;
  state.swarm_cumul = 0;
  state.swarm_size = 0;
  state.epsilon = LUCIOLE_ADJUST_EPSILON;
  state.enabled = LUCIOLE_ON;

  //
  // XBee initialization
  //

  xbee_init(&message);
}

/**
 * Boucle principale de l'Arduino.
 */
void loop() {
  if (state.enabled == LUCIOLE_ON) {
    state.start_at = millis();

    //
    // Émission de lumière.
    // Bon ok, plus d'électrons que de photons...
    //

    flash();

    //
    // Diffusion de l'adresse si besoin.
    //

    xbee_spread();

    //
    // Phase de synchronisation.
    //

    sync();

    //
    // Phase d'ajustement.
    //

    adjust();
  }
  else {
    listen(1000);
  }
}
