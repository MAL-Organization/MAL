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

#include "mal_hspec_gnu_timer.h"
#include "std/mal_bool.h"
#include "std/mal_stdlib.h"
#include "std/mal_stdint.h"
#include "mal_hspec_gnu_gpio.h"
#include "std/mal_defs.h"
#include <pthread.h>
#include <unistd.h>

typedef struct {
	mal_timer_e timer;
	float frequency;
	mal_timer_callback_t callack;
	void *callback_handle;
	pthread_t thread;
	volatile bool active;
	volatile uint32_t count;
	mal_timer_input_capture_callback_t intput_capture_cb[MAL_GPIO_PORT_SIZE][MAL_HSPEC_GNU_GPIO_PORT_SIZE];
    void* intput_capture_cb_handles[MAL_GPIO_PORT_SIZE][MAL_HSPEC_GNU_GPIO_PORT_SIZE];
} gnu_timer_s;

static void* timer_thread(void *handle);
static void count_timer_callback(void *handle);

static mal_timer_e available_timers[MAL_TIMER_SIZE];
static gnu_timer_s gnu_timers[MAL_TIMER_SIZE];

bool mal_hspec_gnu_timer_emulated = true;

void mal_hspec_gnu_timer_restore_default_emulation(void) {
	mal_hspec_gnu_timer_emulated = true;
}

mal_error_e mal_timer_get_valid_timers(const mal_timer_e **timers, uint8_t *size) {
	// Initialize timers
	for (int i = 0; i < MAL_TIMER_SIZE; i++) {
		available_timers[i] = (mal_timer_e)i;
	}
	// Set values
	*timers = available_timers;
	*size = MAL_TIMER_SIZE;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_direct_init_task(mal_timer_init_task_s *init, const void *direct_init, mal_timer_s *handle) {
	MAL_DEFS_UNUSED(direct_init);
    return mal_timer_init_task(init, handle);
}

mal_error_e mal_timer_init_task(mal_timer_init_task_s *init, mal_timer_s *handle) {
	// Save timer parameters
	gnu_timers[init->timer].timer = init->timer;
	gnu_timers[init->timer].frequency = MAL_TYPES_MAL_HERTZ_TO_HERTZ(init->frequency);
	gnu_timers[init->timer].callack = init->callback;
	gnu_timers[init->timer].callback_handle = init->callback_handle;
	// Set handle
    handle->timer = init->timer;
    handle->used = true;
	// Set timer active
	gnu_timers[init->timer].active = true;
	// Create thread
	int pthread_result;
    pthread_result = pthread_create(&gnu_timers[init->timer].thread, NULL, &timer_thread, &gnu_timers[init->timer]);
    if (pthread_result) {
		return MAL_ERROR_INIT_FAILED;
	}

	return MAL_ERROR_OK;
}

static void* timer_thread(void *handle) {
	// Save timer
	gnu_timer_s *timer = (gnu_timer_s*)handle;
	// Compute sleep time
	struct timespec sleep_time;
	float period = 1.0f / timer->frequency;
	sleep_time.tv_sec = (time_t)(period);
	sleep_time.tv_nsec = (long)(period * 1000000000.0f);
	// Run loop
	while (timer->active) {
		// Sleep
        nanosleep(&sleep_time, NULL);
		// Execute callback
		if (timer->active && NULL != timer->callack) {
			timer->callack(timer->callback_handle);
		}
	}
	return 0;
}

mal_error_e mal_timer_free(mal_timer_s *handle) {
	// Set timer as not active
	gnu_timers[handle->timer].active = false;
	// Join thread
	pthread_join(gnu_timers[handle->timer].thread, NULL);

	return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_count(mal_timer_init_count_s *init, mal_timer_s *handle) {
	if (mal_hspec_gnu_timer_emulated) {
		mal_timer_init_task_s task_init;
		task_init.timer = init->timer;
		task_init.frequency = init->frequency;
		task_init.delta = 0;
		task_init.callback = &count_timer_callback;
		task_init.callback_handle = &gnu_timers[init->timer].timer;
		return mal_timer_init_task(&task_init, handle);
	}
	// Time is not emulated
	handle->timer = init->timer;
	handle->used = true;
	gnu_timers[init->timer].timer = init->timer;
	gnu_timers[init->timer].frequency = MAL_TYPES_MAL_HERTZ_TO_HERTZ(init->frequency);
	gnu_timers[init->timer].count = 0;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_input_count(mal_timer_init_count_input_s *init, mal_timer_s *handle) {
    handle->timer = init->timer;
    handle->used = true;
    gnu_timers[init->timer].timer = init->timer;
    gnu_timers[init->timer].count = 0;
    return MAL_ERROR_OK;
}

uint32_t mal_hspec_gnu_timer_get_count(mal_timer_e timer) {
	return gnu_timers[timer].count;
}

void mal_hspec_gnu_timer_set_count(mal_timer_e timer, uint32_t count) {
	gnu_timers[timer].count = count;
}

static void count_timer_callback(void *handle) {
    mal_timer_e timer = *((mal_timer_e*)handle);
	gnu_timers[timer].count++;
}

mal_error_e mal_timer_get_resolution(mal_timer_e timer, uint8_t *resolution) {
    MAL_DEFS_UNUSED(timer);
	*resolution = 32;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_get_count_frequency(mal_timer_s *handle, mal_hertz_t *frequency) {
	*frequency = MAL_TYPES_HERTZ_TO_MAL_HERTZ(gnu_timers[handle->timer].frequency);
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_get_count(mal_timer_s *handle, uint64_t *count) {
	*count = gnu_timers[handle->timer].count;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_input_capture(mal_timer_init_intput_capture_s *init, mal_timer_s *handle,
                                         mal_timer_input_capture_s *input_capture_handle) {
    MAL_DEFS_UNUSED(input_capture_handle);
	// Save timer parameters
	gnu_timers[init->timer].timer = init->timer;
	gnu_timers[init->timer].frequency = init->frequency;
	gnu_timers[init->timer].intput_capture_cb[init->port][init->pin] = init->callback;
    gnu_timers[init->timer].intput_capture_cb_handles[init->port][init->pin] = init->callback_handle;
	// Set handle
    handle->timer = init->timer;
    handle->used = true;
	// Set timer active
	gnu_timers[init->timer].active = true;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_gnu_timer_do_input_capture_callback(mal_timer_e timer,  mal_gpio_port_e port, uint8_t pin,
                                                          uint64_t value) {
    void *handle = gnu_timers[timer].intput_capture_cb_handles[port][pin];
	gnu_timers[timer].intput_capture_cb[port][pin](handle, value);
    return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_pwm(mal_timer_init_pwm_s *init, mal_timer_s *handle, mal_timer_pwm_s *pwm_handle) {
    MAL_DEFS_UNUSED(pwm_handle);
    handle->timer = init->timer;
    handle->used = true;
    gnu_timers[init->timer].timer = init->timer;
    gnu_timers[init->timer].count = 0;
    return MAL_ERROR_OK;
}
mal_error_e mal_timer_set_frequency(mal_timer_s *handle, mal_hertz_t frequency, mal_hertz_t delta) {
    MAL_DEFS_UNUSED(delta);
    gnu_timers[handle->timer].frequency = frequency;
    return MAL_ERROR_OK;
}

mal_error_e mal_timer_fast_set_frequency(mal_timer_s *handle, mal_hertz_t frequency, mal_hertz_t delta) {
    return mal_timer_set_frequency(handle, frequency, delta);
}

mal_error_e mal_timer_set_pwm_duty_cycle(mal_timer_pwm_s *handle, mal_ratio_t duty_cycle) {
    handle->duty_cycle = duty_cycle;
    return MAL_ERROR_OK;
}

MAL_DEFS_INLINE void mal_timer_disable_interrupt(mal_timer_s *handle, mal_timer_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
	MAL_DEFS_UNUSED(state);
}

MAL_DEFS_INLINE void mal_timer_restore_interrupt(mal_timer_s *handle, mal_timer_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(state);
}
