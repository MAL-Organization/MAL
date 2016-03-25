/*
 * mal_hspec_mingw_timer.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */

#include <pthread.h>
#include <pthread_time.h>

#include "mal_hspec_mingw_timer.h"
#include "std/mal_bool.h"
#include "std/mal_stdlib.h"

typedef struct {
	mal_hspec_timer_e timer;
	float frequency;
	mal_hspec_timer_callback_t callack;
	pthread_t thread;
	volatile bool active;
} mingw_timer_s;

static void *timer_thread(void *ptr);

static mal_hspec_timer_e available_timers[MAL_HSPEC_TIMER_SIZE];
static mingw_timer_s mingw_timers[MAL_HSPEC_TIMER_SIZE];

mal_error_e mal_hspec_mingw_get_valid_timers(const mal_hspec_timer_e **timers, uint8_t *size) {
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
	int result = pthread_create(&mingw_timers[timer].thread, NULL, &timer_thread, (void*)(&mingw_timers[timer]));
	if (result) {
		return MAL_ERROR_INIT_FAILED;
	}

	return MAL_ERROR_OK;
}

static void *timer_thread(void *ptr) {
	// Save timer
	mingw_timer_s *timer = (mingw_timer_s*)ptr;
	// Compute sleep time
	struct timespec sleep_time;
	float period = 1.0f / timer->frequency;
	sleep_time.tv_sec = (time_t)period;
	sleep_time.tv_nsec = (long)(1000000.0f * period);
	// Run loop
	while (timer->active) {
		// Sleep
		nanosleep(&sleep_time, NULL);
		// Execute callback
		if (timer->active && NULL != timer->callack) {
			timer->callack(timer->timer);
		}
	}
	return NULL;
}

mal_error_e mal_hspec_mingw_timer_free(mal_hspec_timer_e timer) {
	// Set timer as not active
	mingw_timers[timer].active = false;
	// Join thread
	pthread_join(mingw_timers[timer].thread, NULL);

	return MAL_ERROR_OK;
}
