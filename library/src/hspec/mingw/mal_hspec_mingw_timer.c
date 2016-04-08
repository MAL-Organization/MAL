/*
 * mal_hspec_mingw_timer.c
 *
 *  Created on: Mar 24, 2016
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

#include "mal_hspec_mingw_timer.h"
#include "std/mal_bool.h"
#include "std/mal_stdlib.h"
#include "std/mal_stdint.h"
#include <windows.h> // Leave this include last, windows defines the word interface and it creates conflicts.

typedef struct {
	mal_hspec_timer_e timer;
	float frequency;
	mal_hspec_timer_callback_t callack;
	HANDLE thread;
	volatile bool active;
	volatile uint32_t count;
} mingw_timer_s;

static DWORD WINAPI timer_thread(LPVOID lpParameter);
static mal_error_e count_timer_callback(mal_hspec_timer_e timer);

static mal_hspec_timer_e available_timers[MAL_HSPEC_TIMER_SIZE];
static mingw_timer_s mingw_timers[MAL_HSPEC_TIMER_SIZE];

mal_error_e mal_hspec_mingw_get_valid_timers(const mal_hspec_timer_e **timers, uint8_t *size) {
	// Initialize timers
	for (int i = 0; i < MAL_HSPEC_TIMER_SIZE; i++) {
		available_timers[i] = i;
	}
	// Set values
	*timers = available_timers;
	*size = MAL_HSPEC_TIMER_SIZE;
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_timer_init(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_callback_t callback) {
	// Save timer parameters
	mingw_timers[timer].timer = timer;
	mingw_timers[timer].frequency = frequency;
	mingw_timers[timer].callack = callback;
	// Set timer active
	mingw_timers[timer].active = true;
	// Create thread
	mingw_timers[timer].thread = CreateThread(NULL, /*Default security settings*/
											  0, /*Default stack size*/
											  &timer_thread,
											  &mingw_timers[timer],
											  0 /*Thread starts now*/,
											  NULL /*No thread identifier*/);
	if (NULL == mingw_timers[timer].thread) {
		return MAL_ERROR_INIT_FAILED;
	}

	return MAL_ERROR_OK;
}

static DWORD WINAPI timer_thread(LPVOID lpParameter) {
	// Save timer
	mingw_timer_s *timer = (mingw_timer_s*)lpParameter;
	// Compute sleep time
	DWORD sleep_time;
	float period = 1.0f / timer->frequency;
	sleep_time = (long)(1000.0f * period);
	// Run loop
	while (timer->active) {
		// Sleep
		Sleep(sleep_time);
		// Execute callback
		if (timer->active && NULL != timer->callack) {
			timer->callack(timer->timer);
		}
	}
	return 0;
}

mal_error_e mal_hspec_mingw_timer_free(mal_hspec_timer_e timer) {
	// Set timer as not active
	mingw_timers[timer].active = false;
	// Join thread
	WaitForSingleObject(mingw_timers[timer].thread, INFINITE);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_timer_count_init(mal_hspec_timer_e timer, float frequency) {
	return mal_hspec_mingw_timer_init(timer, frequency, 0, &count_timer_callback);
}

static mal_error_e count_timer_callback(mal_hspec_timer_e timer) {
	mingw_timers[timer].count++;
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_timer_get_resolution(mal_hspec_timer_e timer, uint8_t *resolution) {
	*resolution = 32;
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_timer_get_count_frequency(mal_hspec_timer_e timer, float *frequency) {
	*frequency = mingw_timers[timer].frequency;
	return MAL_ERROR_OK;
}
