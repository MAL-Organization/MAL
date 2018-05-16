/*
 * Copyright (c) 2018 Olivier Allaire
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

#include "mal_hspec_gnu_adc.h"
#include "std/mal_stdlib.h"
#include "power/mal_power.h"

typedef struct {
	mal_adc_e adc;
	mal_adc_init_s init;
	mal_adc_read_callback_t callback;
	float value;
} adc_info_s;

static adc_info_s adc_array[MAL_ADC_SIZE];

mal_error_e mal_adc_init(mal_adc_init_s *init) {
	// Save info
	adc_array[init->adc].init = *init;

	return MAL_ERROR_OK;
}

mal_error_e mal_adc_read_bits(mal_adc_e adc, uint64_t *value) {
	mal_volts_t vdda;
	mal_power_get_rail_voltage(MAL_POWER_RAIL_VDDA, &vdda);
	// Compute ratio
	float ratio = adc_array[adc].value / MAL_TYPES_MAL_VOLTS_TO_VOLTS(vdda);
	// Get resolution
	uint8_t resolution;
	mal_adc_resolution(adc, &resolution);
	// Compute max int
	uint64_t max_adc_value = (((uint64_t)1) << resolution) - 1;
	// Convert to int
	*value = (uint64_t)(ratio * (float)max_adc_value);

	return MAL_ERROR_OK;
}

mal_error_e mal_adc_resolution(mal_adc_e adc, uint8_t *resolution) {
	*resolution = adc_array[adc].init.bit_resolution;
	return MAL_ERROR_OK;
}

void mal_hspec_gnu_adc_set_value(mal_adc_e adc, float value) {
	adc_array[adc].value = value;
}

mal_error_e mal_adc_async_read(mal_adc_e adc, mal_adc_read_callback_t callback) {
	if (NULL == adc_array[adc].callback) {
		adc_array[adc].callback = callback;
		return MAL_ERROR_OK;
	}
	return MAL_ERROR_HARDWARE_UNAVAILABLE;
}

void mal_hspec_gnu_adc_do_async(mal_adc_e adc) {
	if (NULL != adc_array[adc].callback) {
		// Fetch callback
		mal_adc_read_callback_t cb = adc_array[adc].callback;
		adc_array[adc].callback = NULL;
		// Fetch value
		uint64_t value;
		mal_adc_read_bits(adc, &value);
		// Execute
		cb(adc, value);
	}
}

bool mal_hspec_gnu_adc_peek_async(mal_adc_e adc) {
	return adc_array[adc].callback != NULL;
}

MAL_DEFS_INLINE bool mal_adc_disable_interrupt(mal_adc_e adc) {
    return false;
}

MAL_DEFS_INLINE void mal_adc_enable_interrupt(mal_adc_e adc, bool active) {
    // Nothing to do
}
