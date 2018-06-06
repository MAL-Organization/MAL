/*
 * mal_adc.c
 *
 *  Created on: Sep 11, 2015
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

#include "mal_adc.h"
#include "power/mal_power.h"

mal_error_e mal_adc_read_volts(mal_adc_s *handle, mal_volts_t *value) {
	mal_error_e result;
	// Read bit value
	uint64_t bit_value;
	result = mal_adc_read_bits(handle, &bit_value);
	if (MAL_ERROR_OK != result) {
		return result;
	}

	return mal_adc_bits_to_volts(handle, bit_value, value);
}

mal_error_e mal_adc_bits_to_volts(mal_adc_s *handle, uint64_t bit_value, mal_volts_t *value) {
	mal_error_e result;
	// Get ADC resolution
	uint8_t resolution;
	result = mal_adc_resolution(handle, &resolution);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Get VDDA value
	mal_volts_t vdda_value;
	result = mal_power_get_rail_voltage(MAL_POWER_RAIL_VDDA, &vdda_value);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Compute maximum value
	uint64_t maximum_value = (((uint64_t)1) << resolution) - 1;
	// Compute value
	*value = ((mal_volts_t)bit_value * vdda_value) / (mal_volts_t)maximum_value;
	return MAL_ERROR_OK;
}
