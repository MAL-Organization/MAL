/*
 * mal_timer.c
 *
 *  Created on: May 25, 2015
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
 *
 * This file is part of MAL.
 *
 * MAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "timer/mal_timer.h"
#include "std/mal_bool.h"
#include "hspec/mal_hspec.h"

typedef struct {
	mal_hspec_timer_mode_e mode;
	bool is_available;
	// Tick timer variables
	uint64_t frequency;
	volatile uint64_t tick_counter;

} timer_state_s;

void mal_timer_init(void);

static mal_error_e get_available_timer(mal_hspec_timer_e *timer);

static timer_state_s timer_states[MAL_HSPEC_TIMER_SIZE];

mal_error_e mal_timer_init_tick(mal_hspec_timer_e timer, float frequency, uint8_t precision, mal_hspec_timer_e *handle) {
	mal_error_e result;
	// Check if timer is specified
	if (MAL_HSPEC_TIMER_ANY == timer) {
		result = get_available_timer(&timer);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	}
	// Reserve timer
	if (timer_states[timer].is_available) {
		timer_states[timer].is_available = false;
		timer_states[timer].mode = MAL_HSPEC_TIMER_MODE_TICK;
		*handle = timer;
	} else {
		return MAL_ERROR_HARDWARE_UNAVAILABLE;
	}
	// Check precision
	if (precision > 100) {
		precision = 100;
	}
	// Initialise timer
	result = mal_hspec_init_tick(timer, frequency, precision, &timer_states[timer].tick_counter);

	return result;
}

mal_error_e get_available_timer(mal_hspec_timer_e *timer) {
	int i;
	for (i = 0; i < MAL_HSPEC_TIMER_SIZE; i++) {
		if (timer_states[i].is_available) {
			*timer = i;
			return MAL_ERROR_OK;
		}
	}

	return MAL_ERROR_HARDWARE_UNAVAILABLE;
}

void mal_timer_init(void) {
	mal_hspec_timer_e i;
	for (i = 0; i < MAL_HSPEC_TIMER_SIZE; i++) {
		if (MAL_ERROR_OK == mal_hspec_is_timer_valid(i)) {
			timer_states[i].is_available = true;
		} else {
			timer_states[i].is_available = false;
		}
	}
}

uint64_t mal_timer_get_tick(mal_hspec_timer_e handle) {
	uint64_t tick;
	mal_hspec_disable_timer_interrupt(handle);
	tick = timer_states[handle].tick_counter;
	mal_hspec_enable_timer_interrupt(handle);
	return tick;
}
