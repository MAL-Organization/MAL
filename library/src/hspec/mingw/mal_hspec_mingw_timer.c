/*
 * mal_hspec_mingw_timer.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */


#include "mal_hspec_mingw_timer.h"
#include "std/mal_bool.h"
#include "std/mal_stdlib.h"
#include <windows.h> // Leave this include last, windows defines the word interface and it creates conflicts.

typedef struct {
	mal_hspec_timer_e timer;
	float frequency;
	mal_hspec_timer_callback_t callack;
	HANDLE thread;
	volatile bool active;
} mingw_timer_s;

static DWORD WINAPI timer_thread(LPVOID lpParameter);

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
