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

// essaim
uint16_t essaim[LUCIOLE_VIEW];
unsigned long essaim_time[LUCIOLE_VIEW];
uint8_t essaim_offset = 0;
uint8_t essaim_size = 0;

unsigned long cycle_start;
bool new_cycle = true;
bool new_day, new_night;


void process_command() {
	uint16_t from;
	cmd_t *command = xbee_receive(&from, 1);
	if (!command) return;
	if (command->cmd == CMD_RESET) {
		essaim_offset = 0;
		essaim_size = 0;
		delay(random(LUCIOLE_CYCLE_LENGTH));
		new_cycle = true;
		return;
	}
	if (command->cmd == CMD_SYNC) {
		int i = essaim_index(from);
		if (i >= 0) {
			essaim_time[i] = millis();
		}
		else if (essaim_size < LUCIOLE_VIEW || random(1000) < LUCIOLE_ADD_IN_SWARM_PROB * 1000) {
			add_to_essaim(from);
		}
	}
}


void setup() {
	pinMode(CLED, OUTPUT);

	HELPER_BLINK(CLED, 150, 5);

	pinMode(HLED, OUTPUT);
	pinMode(LLED, OUTPUT);
	// pinMode(6, OUTPUT);
	// pinMode(7, OUTPUT);
	// pinMode(11, OUTPUT);

  //
  // XBee initialization
  //

	xbee_init(&message);
	new_cycle = true;
}

void loop() {
	if (new_cycle) {
		HELPER_DIGITAL_WRITE(LOW, 0);
		xbee_transmit();
		cycle_start = millis();
		new_cycle = false;
		new_day = new_night = true;
	}
	process_command();

	int t = millis() - cycle_start;
	if (t < 255) {
		// dawn
		HELPER_ANALOG_WRITE(t, 0);
	}
	else if (t < LUCIOLE_FLASH_LENGTH - 255) {
		// day
		if (new_day) {
			HELPER_DIGITAL_WRITE(HIGH, 0);
			new_day = false;
		}
	}
	else if (t < LUCIOLE_FLASH_LENGTH) {
		// twilight
		HELPER_ANALOG_WRITE(LUCIOLE_FLASH_LENGTH - t, 0);
	}
	else if (t < LUCIOLE_FLASH_LENGTH + LUCIOLE_CHARGE_LENGTH) {
		// night
		if (new_night) {
			HELPER_DIGITAL_WRITE(LOW, 0);
			new_night = false;
		}
	}
	else if (t >= LUCIOLE_CYCLE_LENGTH || essaim_flashing_count() >= LUCIOLE_JUMP_THRESHOLD) {
		new_cycle = true;
	}
}

int essaim_flashing_count() {
	int count = 0;
	unsigned long now = millis();
	for (int i = 0; i < essaim_size; i++) {
		if ((now - essaim_time[i]) % LUCIOLE_CYCLE_LENGTH < LUCIOLE_FLASH_LENGTH)
			count++;
	}
	return count;
}

int essaim_index(uint16_t addr) {
	for (uint8_t i = 0; i < essaim_size; i++) {
		if (essaim[i] == addr)
			return i;
	}
	return -1;
}

void add_to_essaim(uint16_t addr) {
	essaim[essaim_offset] = addr;
	essaim_time[essaim_offset] = millis();
	essaim_offset = (essaim_offset + 1) % LUCIOLE_VIEW;
	essaim_size = min(LUCIOLE_VIEW, essaim_size + 1);
}


void error_mode(uint8_t error) {
	while (1) {
		HELPER_BLINK(CLED, 300, error);
		delay(1000);
	}
}
