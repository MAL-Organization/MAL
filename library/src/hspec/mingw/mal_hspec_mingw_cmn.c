/*
 * mal_hspec_mingw_cmn.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
 *
 * This file is part of MAL.
 *
 * MAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "hspec/mal_hspec_cmn.h"

static mal_hspec_gpio_s valid_ios[MAL_HSPEC_GPIO_PORT_SIZE][PORT_SIZE];
static mal_hspec_gpio_port_e valid_ports[MAL_HSPEC_GPIO_PORT_SIZE];
static uint64_t valid_pins[MAL_HSPEC_GPIO_PORT_SIZE];

void mal_hspec_mingw_cmn_valid_ios(const mal_hspec_gpio_s **ios, uint8_t *size) {
	// Initialise IOs
	for (int i = 0; i < MAL_HSPEC_GPIO_PORT_SIZE; i++) {
		for (int j = 0; j < PORT_SIZE; j++) {
			valid_ios[i][j].port = i;
			valid_ios[i][j].pin = j;
		}
	}
	// Set IOs
	*ios = (mal_hspec_gpio_s*)valid_ios;
	*size = MAL_HSPEC_GPIO_PORT_SIZE * PORT_SIZE;
}

void mal_hspec_mingw_cmn_valid_ports(const mal_hspec_gpio_port_e **ports, uint8_t *size) {
	// Initialise Ports
	for(int i = 0; i < MAL_HSPEC_GPIO_PORT_SIZE; i++) {
		valid_ports[i] = i;
	}

	*ports = valid_ports;
	*size = MAL_HSPEC_GPIO_PORT_SIZE;
}

void mal_hspec_mingw_cmn_valid_pins(const uint64_t **pins) {
	// Initialise Pins
	for(int i = 0; i < MAL_HSPEC_GPIO_PORT_SIZE; i++) {
		valid_pins[i] = ((uint64_t)1 << (uint64_t)PORT_SIZE) - (uint64_t)1;
	}
	*pins = valid_pins;
}
