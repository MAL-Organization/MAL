/*
 * mal_dac.c
 *
 *  Created on: Oct 25, 2016
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

#include "mal_dac.h"
#include "power/mal_power.h"

mal_error_e mal_dac_write_volts(mal_dac_s *handle, mal_volts_t value) {
	mal_error_e result;
	// Convert value
	uint64_t bit_value;
	result = mal_dac_volts_to_bits(handle, value, &bit_value);
	if (MAL_ERROR_OK != result) {
		return result;
	}

	return mal_dac_write_bits(handle, bit_value);
}

mal_error_e mal_dac_volts_to_bits(mal_dac_s *handle, mal_volts_t value, uint64_t *bit_value) {
	mal_error_e result;
	// Get DAC resolution
	uint8_t resolution;
	result = mal_dac_resolution(handle, &resolution);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Get VDDA voltage
	mal_volts_t vdda;
    result = mal_power_get_rail_voltage(MAL_POWER_RAIL_VDDA, &vdda);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Compute maximum value
	uint64_t maximum_value = (((uint64_t)1) << resolution) - 1;
	// Compute bit value
	*bit_value = (uint64_t)((value * (mal_volts_t)maximum_value) / vdda);
	if (*bit_value > maximum_value) {
		*bit_value = maximum_value;
	}
	return MAL_ERROR_OK;
}
