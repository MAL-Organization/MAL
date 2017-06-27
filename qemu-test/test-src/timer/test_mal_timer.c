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

#include <stdio.h>

#include "uCUnit-v1.0.h"
#include "timer/mal_timer.h"
#include "hspec/test_mal_hspec.h"
#include "hspec/mal_hspec.h"
#include "utils/char_buffer.h"

/**
 * Since not all implementations of init capture may be able to test the
 * capture event without external hardware, we will let the hardware specific
 * tests test this part. But since the timer is initialized in the hardware
 * abstracted section, we must forward test info.
 */
static volatile test_mal_hspec_timer_input_capture_t input_capture_info;

static mal_error_e input_capture_callback(mal_timer_e timer, uint64_t count) {
	input_capture_info.timer = timer;
	input_capture_info.count = count;
	return MAL_ERROR_OK;
}

/**
 * Test the timer tick initializer. Makes sure that the timer init algorithms
 * can still find proper parameters.
 */
static void test_mal_timer_init_tick_1khz(void) {
	uint8_t i;
	mal_error_e result;
	const mal_timer_e *timers;
	uint8_t size;

	UCUNIT_TestcaseBegin("test_mal_timer_init_tick_1khz");

	// Get timers to test
	result = mal_hspec_get_valid_timers(&timers, &size);
	UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);

	// Prepare test frequency of 1kHz and delta of 0Hz
	mal_hertz_t test_frequency = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(1000000);
	mal_hertz_t test_delta = 0;

	// Test timers
	if (MAL_ERROR_OK == result) {
		for (i = 0; i < size; i++) {
			sprintf(char_buffer, "Testing timer %i\n", timers[i] + 1);
			UCUNIT_WriteString(char_buffer);
			mal_timer_e test_handle;
			result = mal_timer_init_tick(timers[i],
										 test_frequency,
										 test_delta,
										 &test_handle);
			// Check result
			UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
			// Check timer handle
			UCUNIT_CheckIsEqual(timers[i], test_handle);
			// Execute hardware specific test
			test_mal_hspec_timer_init_tick_1khz(timers[i]);
			// Free timer
			mal_timer_free(timers[i]);
		}
	}

	UCUNIT_TestcaseEnd();
}

/**
 * Same as test_mal_timer_init_tick_1khz, but using the direct initializer. It
 * will also test the mal_timer_get_tick method.
 */
static void test_mal_timer_direct_init_tick_1khz(void) {
	uint8_t i;
	mal_error_e result;
	const mal_timer_e *timers;
	uint8_t size;

	UCUNIT_TestcaseBegin("test_mal_timer_direct_init_tick_1khz");

	// Get timers to test
	result = mal_hspec_get_valid_timers(&timers, &size);
	UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);

	// Prepare test frequency of 1kHz and delta of 0Hz
	mal_hertz_t test_frequency = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(1000000);
	mal_hertz_t test_delta = 0;

	// Test timers
	if (MAL_ERROR_OK == result) {
		for (i = 0; i < size; i++) {
			sprintf(char_buffer, "Testing timer %i\n", timers[i] + 1);
			UCUNIT_WriteString(char_buffer);
			// Get direct init
			const void *direct_init;
			test_mal_hspec_timer_get_1khz_direct_init(timers[i], &direct_init);
			// Initialize timer
			mal_timer_e test_handle;
			result = mal_timer_direct_init_tick(timers[i],
												test_frequency,
												test_delta,
												direct_init,
												&test_handle);
			// Check result
			UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
			// Check timer handle
			UCUNIT_CheckIsEqual(timers[i], test_handle);
			// Execute hardware specific test
			test_mal_hspec_timer_direct_init_tick_1khz(timers[i]);
			// Get first tick value
			uint64_t test_tick_count = mal_timer_get_tick(timers[i]);
			// Wait
			bool tick_pass = false;
			for (int wait_count = 0; wait_count < 10000; wait_count++) {
				// Read count again
				uint64_t later_test_tick_count = mal_timer_get_tick(timers[i]);
				if (later_test_tick_count != test_tick_count) {
					tick_pass = true;
					break;
				}
			}
			UCUNIT_CheckIsEqual(true, tick_pass);
			// Free timer
			mal_timer_free(timers[i]);
		}
	}

	UCUNIT_TestcaseEnd();
}

/**
 * Test the timer count functionality. Makes sure that the timer init
 * algorithms can still find proper parameters.
 */
static void test_mal_timer_count_1khz(void) {
	uint8_t i;
	mal_error_e result;
	const mal_timer_e *timers;
	uint8_t size;

	UCUNIT_TestcaseBegin("test_mal_timer_count_1khz");

	// Get timers to test
	result = mal_hspec_get_valid_timers(&timers, &size);
	UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);

	// Prepare test frequency of 1kHz and delta of 0Hz
	mal_hertz_t test_frequency = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(1000000);

	// Test timers
	if (MAL_ERROR_OK == result) {
		for (i = 0; i < size; i++) {
			sprintf(char_buffer, "Testing timer %i\n", timers[i] + 1);
			UCUNIT_WriteString(char_buffer);
			mal_timer_e test_handle;
			result = mal_timer_init_count(timers[i],
										  test_frequency,
										  &test_handle);
			// Check result
			UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
			// Check timer handle
			UCUNIT_CheckIsEqual(timers[i], test_handle);
			// Execute hardware specific test
			test_mal_hspec_timer_init_count_1khz(timers[i]);
			// Get first count value
			uint64_t test_tick_count;
			result = mal_timer_get_count(timers[i], &test_tick_count);
			UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
			// Wait
			bool count_pass = false;
			mal_error_e count_result = MAL_ERROR_OK;
			for (int wait_count = 0; wait_count < 10000; wait_count++) {
				// Read count again
				uint64_t later_test_tick_count;
				result = mal_timer_get_count(timers[i], &later_test_tick_count);
				// Check result
				if (MAL_ERROR_OK != result) {
					count_result = result;
				}
				// Check if count changed
				if (later_test_tick_count != test_tick_count) {
					count_pass = true;
					break;
				}
			}
			UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
			UCUNIT_CheckIsEqual(true, count_pass);
			// Free timer
			mal_timer_free(timers[i]);
		}
	}

	UCUNIT_TestcaseEnd();
}

/**
 * Test PWM initialization and duty cycle functionalities.
 */
static void test_mal_timer_pwm_1khz(void) {
	uint8_t timer_index;
	uint8_t io_index;
	mal_error_e result;
	const mal_timer_e *timers;
	uint8_t timers_size;
	const mal_hspec_gpio_s *ios;
	uint8_t ios_size;

	UCUNIT_TestcaseBegin("test_mal_timer_pwm_1khz");

	// Get timers to test
	result = mal_hspec_get_valid_timers(&timers, &timers_size);
	UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);

	// Prepare test frequency of 1kHz and delta of 0Hz
	mal_hertz_t test_frequency = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(1000000);
	mal_hertz_t test_delta = 0;
	mal_ratio_t test_duty_cycle = MAL_TYPES_RATIO_NORMALIZER / 2;

	// Test timers
	if (MAL_ERROR_OK == result) {
		for (timer_index = 0; timer_index < timers_size; timer_index++) {
			// Fetch IOs
			result = mal_hspec_get_valid_pwm_ios(timers[timer_index], &ios, &ios_size);
			if (MAL_ERROR_OK != result) {
				continue;
			}
			// Test IOs
			for (io_index = 0; io_index < ios_size; io_index++) {
				sprintf(char_buffer,
						"Testing timer %i on port %i of io %i\n",
						timers[timer_index] + 1,
						ios[io_index].port,
						ios[io_index].pin);
				UCUNIT_WriteString(char_buffer);
				mal_timer_pwm_init_s pwm_init;
				pwm_init.frequency = test_frequency;
				pwm_init.delta = test_delta;
				pwm_init.timer = timers[timer_index];
				pwm_init.pwm_io = &ios[io_index];
				result = mal_timer_init_pwm(&pwm_init);
				// Check result
				UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
				// Execute hardware specific test
				test_mal_hspec_timer_init_pwm_1khz(timers[timer_index]);
				// Set duty cycle
				result = mal_timer_set_pwm_duty_cycle(timers[timer_index],
													  &ios[io_index],
													  test_duty_cycle);
				UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
				// Test duty cycle value
				test_mal_hspec_timer_pwm_50pc_dc_1khz(timers[timer_index], &ios[io_index]);
				// Free timer
				mal_timer_free(timers[timer_index]);
				// Set IO to input to clear settings
				mal_gpio_deinit(&ios[io_index]);
			}
		}
	}

	UCUNIT_TestcaseEnd();
}

/**
 * Test the timer input capture functionality. Makes sure that the timer init
 * algorithms can still find proper parameters.
 */
static void test_mal_timer_input_capture_1khz(void) {
	uint8_t timer_index;
	uint8_t io_index;
	mal_error_e result;
	const mal_timer_e *timers;
	uint8_t timers_size;
	const mal_hspec_gpio_s *ios;
	uint8_t ios_size;

	UCUNIT_TestcaseBegin("test_mal_timer_input_capture_1khz");

	// Get timers to test
	result = mal_hspec_get_valid_timers(&timers, &timers_size);
	UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);

	// Prepare test frequency of 1kHz and delta of 0Hz
	mal_hertz_t test_frequency = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(1000000);

	// Test timers
	if (MAL_ERROR_OK == result) {
		for (timer_index = 0; timer_index < timers_size; timer_index++) {
			// Fetch IOs
			result = mal_hspec_get_valid_input_capture_ios(timers[timer_index], &ios, &ios_size);
			if (MAL_ERROR_OK != result) {
				continue;
			}
			// Test IOs
			for (io_index = 0; io_index < ios_size; io_index++) {
				sprintf(char_buffer,
						"Testing timer %i on port %i of io %i\n",
						timers[timer_index] + 1,
						ios[io_index].port,
						ios[io_index].pin);
				UCUNIT_WriteString(char_buffer);
				mal_timer_intput_capture_init_s ic_init;
				ic_init.timer = timers[timer_index];
				ic_init.input_io = &ios[io_index];
				ic_init.frequency = test_frequency;
				ic_init.input_divider = 1;
				ic_init.input_event = MAL_TIMER_INPUT_BOTH;
				ic_init.callback = &input_capture_callback;
				result = mal_timer_init_input_capture(&ic_init);
				// Check result
				UCUNIT_CheckIsEqual(MAL_ERROR_OK, result);
				// Execute hardware specific test
				test_mal_hspec_timer_input_capture_1khz(timers[timer_index],
														&ios[io_index],
														&input_capture_info);
				// Free timer
				mal_timer_free(timers[timer_index]);
				// Set IO to input to clear settings
				mal_gpio_deinit(&ios[io_index]);
			}
		}
	}

	UCUNIT_TestcaseEnd();
}

void test_mal_timer(void) {
	test_mal_timer_init_tick_1khz();
	test_mal_timer_direct_init_tick_1khz();
	test_mal_timer_count_1khz();
	test_mal_timer_pwm_1khz();
	test_mal_timer_input_capture_1khz();
}

