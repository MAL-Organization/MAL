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
#include "std/mal_math.h"

#ifdef MAL_FLOAT
#define mal_timer_abs fabs
#else
#define	mal_timer_abs abs_int64
#endif

void mal_timer_states_init(void);

static mal_error_e reserve_timer(mal_timer_e timer, mal_timer_e *handle);

static mal_error_e get_available_timer(mal_timer_e *timer);

static mal_error_e timer_tick_callback(mal_timer_e timer);

static mal_error_e mal_timer_internal_init_common(mal_timer_e timer,
												  mal_hertz_t frequency,
												  mal_hertz_t delta,
												  mal_timer_e *handle);

static mal_error_e mal_timer_internal_init(mal_timer_e timer,
										   mal_hertz_t frequency,
										   mal_hertz_t delta,
										   mal_timer_callback_t callback,
										   mal_timer_e *handle);

static mal_error_e mal_timer_internal_direct_init(mal_timer_e timer,
												  mal_hertz_t frequency,
												  mal_hertz_t delta,
												  mal_timer_callback_t callback,
												  const void *direct_init,
												  mal_timer_e *handle);

static mal_timer_state_s timer_states[MAL_TIMER_SIZE];

mal_error_e mal_timer_init_tick(mal_timer_init_tick_s *init, mal_timer_e *handle) {
	return mal_timer_internal_init(init->timer,
								   init->frequency,
								   init->delta,
								   &timer_tick_callback,
								   handle);
}

mal_error_e mal_timer_direct_init_tick(mal_timer_init_tick_s *init, const void *direct_init, mal_timer_e *handle) {
	return mal_timer_internal_direct_init(init->timer,
										  init->frequency,
										  init->delta,
										  &timer_tick_callback,
										  direct_init,
										  handle);
}

static mal_error_e get_available_timer(mal_timer_e *timer) {
	int i;
	for (i = 0; i < MAL_TIMER_SIZE; i++) {
		if (timer_states[i].is_available) {
			*timer = i;
			return MAL_ERROR_OK;
		}
	}

	return MAL_ERROR_HARDWARE_UNAVAILABLE;
}

void mal_timer_states_init(void) {
	mal_timer_e i;
	for (i = 0; i < MAL_TIMER_SIZE; i++) {
		if (MAL_ERROR_OK == mal_timer_is_valid(i)) {
			timer_states[i].is_available = true;
		} else {
			timer_states[i].is_available = false;
		}
	}
}

uint64_t mal_timer_get_tick(mal_timer_e handle) {
	uint64_t tick;
	bool active = mal_timer_disable_interrupt(handle);
	tick = timer_states[handle].tick_counter;
	mal_timer_enable_interrupt(handle, active);
	return tick;
}

mal_error_e mal_timer_free(mal_timer_e timer) {
	mal_error_e result = MAL_ERROR_OK;
	// Free timer
	if (!timer_states[timer].is_available) {
		result = mal_timer_free_unmanaged(timer);
		timer_states[timer].is_available = true;
	}
	return result;
}

mal_error_e mal_timer_init_task(mal_timer_init_s *init, mal_timer_e *handle) {
	return mal_timer_internal_init(init->timer,
								   init->frequency,
								   init->delta,
								   init->callback,
								   handle);
}

static mal_error_e mal_timer_internal_init_common(mal_timer_e timer,
												  mal_hertz_t frequency,
												  mal_hertz_t delta,
												  mal_timer_e *handle) {
	mal_error_e result;
	// Reserve timer
	result = reserve_timer(timer, handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Save info
	timer_states[*handle].frequency = frequency;
	timer_states[*handle].delta = delta;

	return MAL_ERROR_OK;
}

static mal_error_e mal_timer_internal_init(mal_timer_e timer,
										   mal_hertz_t frequency,
										   mal_hertz_t delta,
										   mal_timer_callback_t callback,
										   mal_timer_e *handle) {
	mal_error_e result;
	result = mal_timer_internal_init_common(timer, frequency, delta, handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialise timer
	mal_timer_init_s init;
	init.timer = *handle;
	init.frequency = frequency;
	init.delta = delta;
	init.callback = callback;
	result = mal_timer_init(&init);

	return result;
}

static mal_error_e mal_timer_internal_direct_init(mal_timer_e timer,
												  mal_hertz_t frequency,
												  mal_hertz_t delta,
												  mal_timer_callback_t callback,
												  const void *direct_init,
												  mal_timer_e *handle) {
	mal_error_e result;
	result = mal_timer_internal_init_common(timer, frequency, delta, handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialise timer
	mal_timer_init_s init;
	init.timer = *handle;
	init.frequency = frequency;
	init.delta = delta;
	init.callback = callback;
	result = mal_timer_direct_init(&init, direct_init);

	return result;
}

static mal_error_e reserve_timer(mal_timer_e timer, mal_timer_e *handle) {
	mal_error_e result;
	// Check if timer is specified
	if (MAL_TIMER_ANY == timer) {
		result = get_available_timer(&timer);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	}
	// Reserve timer
	if (timer_states[timer].is_available) {
		timer_states[timer].is_available = false;
		*handle = timer;
	} else {
		return MAL_ERROR_HARDWARE_UNAVAILABLE;
	}

	return MAL_ERROR_OK;
}

static mal_error_e timer_tick_callback(mal_timer_e timer) {
	timer_states[timer].tick_counter++;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_pwm(mal_timer_pwm_init_s *init) {
	mal_error_e result;
	// Reserve timer
	result = reserve_timer(init->timer, &init->timer);
	if (MAL_ERROR_OK != result && MAL_ERROR_HARDWARE_UNAVAILABLE != result) {
		return result;
	}
	// Save info
	timer_states[init->timer].frequency = init->frequency;
	timer_states[init->timer].delta = init->delta;
	// Initialize timer
	return mal_timer_init_pwm_unmanaged(init);
}

mal_error_e mal_timer_get_state(mal_timer_e timer, mal_timer_state_s *state) {
	// Copy timer
	*state = timer_states[timer];
	// Check if timer is valid
	return mal_timer_is_valid(timer);
}

mal_error_e mal_timer_init_count(mal_timer_e timer,
								 mal_hertz_t frequency,
								 mal_timer_e *handle) {
	mal_error_e result;
	// Reserve timer
	result = reserve_timer(timer, handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialize timer
	result = mal_timer_init_count_unmanaged(*handle, frequency);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Save info
	timer_states[*handle].frequency = frequency;
	mal_hertz_t count_frequency;
	result = mal_timer_get_count_frequency(*handle, &count_frequency);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	timer_states[*handle].delta = mal_timer_abs(frequency - count_frequency);

	return MAL_ERROR_OK;
}

mal_error_e mal_timer_get_count_mask(mal_timer_e timer, uint64_t *mask) {
	mal_error_e result;
	// Get timer resolution
	uint8_t resolution;
	result = mal_timer_get_resolution(timer, &resolution);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Compute mask
	if (resolution >= 64) {
		*mask = UINT64_MAX;
	} else {
		*mask = ((((uint64_t)1)<<((uint64_t)resolution)) - ((uint64_t)1));
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_input_capture(mal_timer_intput_capture_init_s *init) {
	mal_error_e result;
	// Reserve timer
	result = reserve_timer(init->timer, &init->timer);
	if (MAL_ERROR_OK != result && MAL_ERROR_HARDWARE_UNAVAILABLE != result) {
		return result;
	}
	// Save info
	timer_states[init->timer].frequency = init->frequency;
	mal_hertz_t count_frequency = init->frequency;
	if (MAL_ERROR_HARDWARE_UNAVAILABLE == result) {
		result = mal_timer_get_count_frequency(init->timer, &count_frequency);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	}
	timer_states[init->timer].delta = mal_timer_abs(init->frequency - count_frequency);
	// Initialize timer
	return mal_timer_init_input_capture_unmanaged(init);
}

mal_error_e mal_timer_is_valid(mal_timer_e timer) {
	uint8_t i;
	mal_error_e result;
	const mal_timer_e *timers;
	uint8_t size;
	result = mal_timer_get_valid_timers(&timers, &size);
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
