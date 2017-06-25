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
#include "std/mal_stdlib.h"

// Clock

extern const mal_hspec_system_clk_s mal_target_system_clock;
mal_error_e mal_hspec_initialise_system_clk(void) {
	return mal_hspec_set_system_clk(&mal_target_system_clock);
}

extern uint64_t mal_external_clk_freq;
uint64_t mal_hspec_get_external_clk_freq(void) {
	return mal_external_clk_freq;
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

mal_error_e mal_hspec_is_i2c_interface_valid(mal_hspec_i2c_e interface, const mal_gpio_s *scl, const mal_gpio_s *sda) {
	uint8_t i;
	mal_error_e result;
	const mal_gpio_s *scls;
	const mal_gpio_s *sdas;
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

mal_error_e mal_hspec_is_can_interface_valid(mal_hspec_can_e interface, const mal_gpio_s *tx, const mal_gpio_s *rx) {
	uint8_t i;
	mal_error_e result;
	const mal_gpio_s *txs;
	const mal_gpio_s *rxs;
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

mal_error_e mal_hspec_is_spi_interface_valid(mal_hspec_spi_e interface,
											 const mal_gpio_s *mosi,
											 const mal_gpio_s *miso,
											 const mal_gpio_s *clk,
											 const mal_gpio_s *select) {
	mal_error_e result;
	uint8_t i;
	bool found;
	const mal_gpio_s *mosis;
	uint8_t mosis_size;
	const mal_gpio_s *misos;
	uint8_t misos_size;
	const mal_gpio_s *clks;
	uint8_t clks_size;
	const mal_gpio_s *selects;
	uint8_t selects_size;
	// Fetch IOs
	result = mal_hspec_get_valid_spi_ios(interface, &mosis, &mosis_size, &misos, &misos_size, &clks, &clks_size, &selects, &selects_size);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Check mosi IO
	found = false;
	for (i = 0; i < mosis_size; i++) {
		if (mosis[i].pin == mosi->pin && mosis[i].port == mosi->port) {
			found = true;
			break;
		}
	}
	if (!found) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	// Check miso IO
	found = false;
	for (i = 0; i < misos_size; i++) {
		if (misos[i].pin == miso->pin && misos[i].port == miso->port) {
			found = true;
			break;
		}
	}
	if (!found) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	// Check clk IO
	found = false;
	for (i = 0; i < clks_size; i++) {
		if (clks[i].pin == clk->pin && clks[i].port == clk->port) {
			found = true;
			break;
		}
	}
	if (!found) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	// Check select IO
	if (NULL != select) {
		found = false;
		for (i = 0; i < selects_size; i++) {
			if (selects[i].pin == select->pin && selects[i].port == select->port) {
				found = true;
				break;
			}
		}
		if (!found) {
			return MAL_ERROR_HARDWARE_INVALID;
		}
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_is_pwm_valid(mal_hspec_timer_e timer, const mal_gpio_s *gpio) {
	mal_error_e result;
	const mal_gpio_s *ios;
	uint8_t size;
	// Make sure timer is valid
	result = mal_hspec_is_timer_valid(timer);
	// Fetch IOs
	result = mal_hspec_get_valid_pwm_ios(timer, &ios, &size);
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

mal_error_e mal_hspec_is_input_capture_valid(mal_hspec_timer_e timer, const mal_gpio_s *gpio) {
	mal_error_e result;
	const mal_gpio_s *ios;
	uint8_t size;
	// Make sure timer is valid
	result = mal_hspec_is_timer_valid(timer);
	// Fetch IOs
	result = mal_hspec_get_valid_input_capture_ios(timer, &ios, &size);
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

mal_error_e mal_hspec_flash_is_page_valid(uint32_t page) {
	if (page >= mal_hspec_flash_get_page_count()) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_is_dac_valid(mal_hspec_dac_e dac, const mal_gpio_s *gpio) {
	mal_error_e result;
	const mal_gpio_s *ios;
	uint8_t size;
	// Fetch IOs
	result = mal_hspec_get_valid_dac_ios(dac, &ios, &size);
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
