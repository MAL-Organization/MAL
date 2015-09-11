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

extern float mal_external_vdda;

mal_error_e mal_adc_init(mal_hspec_adc_init_s *init) {
	mal_error_e result;
	// Check if IO is valid
	result = mal_hspec_is_adc_valid(init->adc, init->gpio);
	if (MAL_ERROR_OK != result) {
		return result;
	}

	return mal_hspec_adc_init(init);
}

mal_error_e mal_adc_read_volts(mal_hspec_adc_e adc, float *value) {
	mal_error_e result;
	// Read bit value
	uint64_t bit_value;
	result = mal_adc_read_bits(adc, &bit_value);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Get ADC resolution
	uint8_t resolution;
	result = mal_adc_resolution(adc, &resolution);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Compute maximum value
	uint64_t maximum_value = (((uint64_t)1) << resolution) - 1;
	// Compute value
	*value = (((float)bit_value) * mal_external_vdda) / ((float)maximum_value);
	return MAL_ERROR_OK;
}
