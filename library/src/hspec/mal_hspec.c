/*
 * mal_hspec.c
 *
 *  Created on: May 4, 2015
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

#include "std/mal_error.h"
#include "hspec/mal_hspec.h"
#include "hspec/mal_hspec_cmn.h"
#include "std/mal_bool.h"

// Clock

extern const mal_hspec_system_clk_s mal_target_system_clock;
mal_error_e mal_hspec_initialise_system_clk(void) {
	return mal_hspec_set_system_clk(&mal_target_system_clock);
}

extern uint64_t mal_external_clk_freq;
uint64_t mal_hspec_get_external_clk_freq(void) {
	return mal_external_clk_freq;
}

// GPIO

mal_error_e mal_hspec_is_port_valid(mal_hspec_port_e port) {
	// Get valid ports
	mal_error_e result;
	const mal_hspec_port_e *ports;
	uint8_t size;
	result = mal_hspec_get_valid_ports(&ports, &size);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Check if port is valid
	uint8_t i;
	for (i = 0; i < size; i++) {
		if (port == ports[i]) {
			return MAL_ERROR_OK;
		}
	}

	return MAL_ERROR_HARDWARE_INVALID;
}

mal_error_e mal_hspec_is_gpio_valid(mal_hspec_gpio_s *gpio) {
	mal_error_e result;
	const uint64_t *pins;
	result = mal_hspec_get_valid_pins(&pins);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	if ((pins[gpio->port] >> gpio->pin) & 1) {
		return MAL_ERROR_OK;
	}

	return MAL_ERROR_HARDWARE_INVALID;
}

// Timer

mal_error_e mal_hspec_is_timer_valid(mal_hspec_timer_e timer) {
	uint8_t i;
	mal_error_e result;
	const mal_hspec_timer_e *timers;
	uint8_t size;
	result = mal_hspec_get_valid_timers(&timers, &size);
	if (MAL_ERROR_OK != result) {
		return result;
	}

	for (i = 0; i < size; i++) {
		if (timers[i] == timer) {
			return MAL_ERROR_OK;
		}
	}

	return MAL_ERROR_HARDWARE_INVALID;
}

// I2C

mal_error_e mal_hspec_is_i2c_interface_valid(mal_hspec_i2c_e interface, const mal_hspec_gpio_s *scl, const mal_hspec_gpio_s *sda) {
	uint8_t i;
	mal_error_e result;
	const mal_hspec_gpio_s *scls;
	const mal_hspec_gpio_s *sdas;
	uint8_t scls_size;
	uint8_t sdas_size;
	bool found;
	// Fetch IOs
	result = mal_hspec_get_valid_i2c_ios(interface, &scls, &scls_size, &sdas, &sdas_size);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Check for SCL io
	found = false;
	for (i = 0; i < scls_size; i++) {
		if (scls[i].pin == scl->pin && scls[i].port == scl->port) {
			found = true;
			break;
		}
	}
	if (!found) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	// Check for SDA io
	found = false;
	for (i = 0; i < sdas_size; i++) {
		if (sdas[i].pin == sda->pin && sdas[i].port == sda->port) {
			found = true;
			break;
		}
	}
	if (!found) {
		return MAL_ERROR_HARDWARE_INVALID;
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_is_can_interface_valid(mal_hspec_can_e interface, const mal_hspec_gpio_s *tx, const mal_hspec_gpio_s *rx) {
	uint8_t i;
	mal_error_e result;
	const mal_hspec_gpio_s *txs;
	const mal_hspec_gpio_s *rxs;
	uint8_t txs_size;
	uint8_t rxs_size;
	bool found;
	// Fetch IOs
	result = mal_hspec_get_valid_can_ios(interface, &txs, &txs_size, &rxs, &rxs_size);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Check for TX io
	found = false;
	for (i = 0; i < txs_size; i++) {
		if (txs[i].pin == tx->pin && txs[i].port == tx->port) {
			found = true;
			break;
		}
	}
	if (!found) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	// Check for SDA io
	found = false;
	for (i = 0; i < rxs_size; i++) {
		if (rxs[i].pin == rx->pin && rxs[i].port == rx->port) {
			found = true;
			break;
		}
	}
	if (!found) {
		return MAL_ERROR_HARDWARE_INVALID;
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_is_adc_valid(mal_hspec_adc_e adc, const mal_hspec_gpio_s *gpio) {
	mal_error_e result;
	const mal_hspec_gpio_s *ios;
	uint8_t size;
	// Fetch IOs
	result = mal_hspec_get_valid_adc_ios(adc, &ios, &size);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Check io
	uint8_t i;
	bool found = false;
	for (i = 0; i < size; i++) {
		if (ios[i].pin == gpio->pin && ios[i].port == gpio->port) {
			found = true;
			break;
		}
	}
	if (!found) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	return MAL_ERROR_OK;
}
