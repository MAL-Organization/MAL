/*
 * test_mal_timer.c
 *
 *  Created on: Jan 23, 2017
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

#include "uCUnit-v1.0.h"
#include "timer/mal_timer.h"
#include "hspec/test_mal_hspec.h"
#include "hspec/mal_hspec.h"

static void test_mal_timer_init_tick_1khz(void) {
	uint8_t i;
	mal_error_e result;
	const mal_hspec_timer_e *timers;
	uint8_t size;

	UCUNIT_TestcaseBegin("test_mal_timer_init_tick_1khz");

	// Get timers to test
	result = mal_hspec_get_valid_timers(&timers, &size);
	UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);

	// Prepare test frequency and delta
	mal_hspec_timer_value_t test_frequency; // 1kHz
	mal_hspec_timer_value_t test_delta = 0;
#ifdef MAL_FLOAT
	test_frequency = 1000.0f;
#else
	test_frequency = 1000000;
#endif

	// Test timers
	if (MAL_ERROR_OK == result) {
		for (i = 0; i < size; i++) {
			mal_hspec_timer_e test_handle;
			result = mal_timer_init_tick(timers[i],
										 test_frequency,
										 test_delta,
										 &test_handle);
			// Check result
			UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
			// Check timer handle
			UCUNIT_CheckIsEqual(timers[i], test_handle);
			// Execute hardware specific test
			test_mal_hspec_init_tick_1khz(timers[i]);
		}
	}

	UCUNIT_TestcaseEnd();
}

void test_mal_timer(void) {
	test_mal_timer_init_tick_1khz();
}

