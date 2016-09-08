/*
 * mal_hspec_stm32f072rb_gpio.c
 *
 *  Created on: Sep 6, 2016
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

#include "mal_hspec_stm32f072rb_gpio.h"

static const mal_hspec_gpio_port_e valid_ports[] = {
	MAL_HSPEC_GPIO_PORT_A,
	MAL_HSPEC_GPIO_PORT_B,
	MAL_HSPEC_GPIO_PORT_C,
	MAL_HSPEC_GPIO_PORT_D,
	MAL_HSPEC_GPIO_PORT_F
};

static const uint64_t valid_pins[MAL_HSPEC_GPIO_PORT_SIZE] = {
	0b1111111111111111,	// PA[0:15]
	0b1111111111111111, // PB[0:15]
	0b1111111111111111, // PC[0:15]
	0b100,				// PD2
	0,
	0b11			// PF[0:1]
};

mal_error_e mal_hspec_stm32f072rb_get_valid_ports(const mal_hspec_gpio_port_e **ports, uint8_t *size) {
	*ports = valid_ports;
	*size = sizeof(valid_ports) / sizeof(mal_hspec_gpio_port_e);
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f072rb_get_valid_pins(const uint64_t **pins) {
	*pins = valid_pins;
	return MAL_ERROR_OK;
}
