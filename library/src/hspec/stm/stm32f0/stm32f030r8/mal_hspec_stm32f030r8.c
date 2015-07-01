/*
 * mal_hspec_stm32f030r8.c
 *
 *  Created on: May 3, 2015
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
 *
 * This file is part of MAL.
 *
 * MAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mal_hspec_stm32f030r8.h"

static const mal_hspec_port_e valid_ports[] = {
	MAL_GPIO_PORT_A,
	MAL_GPIO_PORT_B,
	MAL_GPIO_PORT_C,
	MAL_GPIO_PORT_D,
	MAL_GPIO_PORT_F
};

static const uint64_t valid_pins[MAL_GPIO_PORT_SIZE] = {
	0b1111111111111111,	// PA[0:15]
	0b1111111111111111, // PB[0:15]
	0b1111111111111111, // PC[0:15]
	0b100,				// PD2
	0,
	0b1111011			// PF[0:1,3:7]
};

mal_error_e mal_hspec_stm32f030r8_get_valid_ports(const mal_hspec_port_e **ports, uint8_t *size) {
	*ports = valid_ports;
	*size = sizeof(valid_ports);
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f030r8_get_valid_pins(const uint64_t **pins) {
	*pins = valid_pins;
	return MAL_ERROR_OK;
}
