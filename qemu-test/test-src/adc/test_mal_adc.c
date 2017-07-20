/*
 * test_mal_adc.c
 *
 *  Created on: Jan 30, 2017
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

#include <stdio.h>

#include "test_mal_adc.h"
#include "adc/mal_adc.h"
#include "uCUnit-v1.0.h"
#include "utils/char_buffer.h"

/**
 * Test the ADC initializer.
 */
static void test_mal_adc_init(void) {
	uint8_t adc_index;
	uint8_t io_index;
	mal_error_e result;
	const mal_gpio_s *ios;
	uint8_t ios_size;

	UCUNIT_TestcaseBegin("test_mal_adc_init");

	// Test ADCs
	for (adc_index = 0; adc_index < MAL_ADC_SIZE; adc_index++) {
		// Fetch IOs
		result = mal_hspec_get_valid_adc_ios(adc_index, &ios, &ios_size);
		if (MAL_ERROR_OK != result) {
			continue;
		}
		// Fetch maximum resolution for this ADC
		uint8_t resolution;
		result = mal_adc_maximum_resolution(adc_index, &resolution);
		UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
		// Test IOs
		for (io_index = 0; io_index < ios_size; io_index++) {
			sprintf(char_buffer,
					"Testing ADC %i on port %i of io %i\n",
					adc_index,
					ios[io_index].port,
					ios[io_index].pin);
			UCUNIT_WriteString(char_buffer);
			mal_adc_init_s adc_init;
			adc_init.adc = adc_index;
			adc_init.bit_resolution = resolution;
			adc_init.gpio = &ios[io_index];
			result = mal_adc_init(&adc_init);
			// Check result
			UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
			// Set IO to input to clear settings
			mal_gpio_deinit(&ios[io_index]);
		}
	}

	UCUNIT_TestcaseEnd();
}

void test_mal_adc(void) {
	test_mal_adc_init();
}

