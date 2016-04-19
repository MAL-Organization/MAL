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

#include "timer/mal_timer.h"
#include "hspec/mal_hspec.h"

void mal_timer_init(void);
static mal_error_e reserve_timer(mal_hspec_timer_e timer, mal_hspec_timer_mode_e mode, mal_hspec_timer_e *handle);
static mal_error_e get_available_timer(mal_hspec_timer_e *timer);
static mal_error_e timer_tick_callback(mal_hspec_timer_e timer);
static mal_error_e mal_timer_internal_init(mal_hspec_timer_e timer, mal_hspec_timer_mode_e mode, float frequency, float delta, mal_hspec_timer_callback_t callback, mal_hspec_timer_e *handle);

static mal_timer_state_s timer_states[MAL_HSPEC_TIMER_SIZE];

mal_error_e mal_timer_init_tick(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_e *handle) {
	return mal_timer_internal_init(timer, MAL_HSPEC_TIMER_MODE_TICK, frequency, delta, &timer_tick_callback, handle);
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
	bool active = mal_timer_disable_interrupt(handle);
	tick = timer_states[handle].tick_counter;
	mal_timer_enable_interrupt(handle, active);
	return tick;
}

mal_error_e mal_timer_free(mal_hspec_timer_e timer) {
	mal_error_e result = MAL_ERROR_OK;
	// Free timer
	if (!timer_states[timer].is_available) {
		result = mal_hspec_timer_free(timer);
		timer_states[timer].is_available = true;
	}
	return result;
}

mal_error_e mal_timer_init_task(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_callback_t callback, mal_hspec_timer_e *handle) {
	return mal_timer_internal_init(timer, MAL_HSPEC_TIMER_MODE_TASK, frequency, delta, callback, handle);
}

static mal_error_e mal_timer_internal_init(mal_hspec_timer_e timer,
										   mal_hspec_timer_mode_e mode,
										   float frequency, float delta,
										   mal_hspec_timer_callback_t callback,
										   mal_hspec_timer_e *handle) {
	mal_error_e result;
	// Reserve timer
	result = reserve_timer(timer, mode, handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Save info
	timer_states[*handle].frequency = frequency;
	timer_states[*handle].delta = delta;
	// Initialise timer
	result = mal_hspec_timer_init(*handle, frequency, delta, callback);

	return result;
}

static mal_error_e reserve_timer(mal_hspec_timer_e timer, mal_hspec_timer_mode_e mode, mal_hspec_timer_e *handle) {
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
		timer_states[timer].mode = mode;
		*handle = timer;
	} else {
		return MAL_ERROR_HARDWARE_UNAVAILABLE;
	}

	return MAL_ERROR_OK;
}

static mal_error_e timer_tick_callback(mal_hspec_timer_e timer) {
	timer_states[timer].tick_counter++;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_pwm(mal_hspec_timer_pwm_init_s *init) {
	mal_error_e result;
	// Check PWM io
	result = mal_hspec_is_pwm_valid(init->timer, init->pwm_io);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Reserve timer
	result = reserve_timer(init->timer, MAL_HSPEC_TIMER_MODE_PWM, &init->timer);
	if (MAL_ERROR_OK != result && MAL_ERROR_HARDWARE_UNAVAILABLE != result) {
		return result;
	}
	// Save info
	timer_states[init->timer].frequency = init->frequency;
	timer_states[init->timer].delta = init->delta;
	// Initialize timer
	return mal_hspec_timer_pwm_init(init);
}

mal_error_e mal_timer_get_state(mal_hspec_timer_e timer, mal_timer_state_s *state) {
	// Copy timer
	*state = timer_states[timer];
	// Check if timer is valid
	return mal_hspec_is_timer_valid(timer);
}
