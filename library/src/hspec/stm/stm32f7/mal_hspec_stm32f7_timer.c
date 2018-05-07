/*
 * mal_hspec_stm32f7_timer.c
 *
 *  Created on: May 7, 2018
 *      Author: olivi
 */

#include "timer/mal_timer.h"

static const mal_timer_e valid_timers[] = {
    MAL_TIMER_1,
    MAL_TIMER_2,
    MAL_TIMER_3,
    MAL_TIMER_4,
    MAL_TIMER_5,
    MAL_TIMER_6,
    MAL_TIMER_7,
    MAL_TIMER_8,
    MAL_TIMER_9,
    MAL_TIMER_10,
    MAL_TIMER_11,
    MAL_TIMER_12,
    MAL_TIMER_13,
    MAL_TIMER_14
};

mal_error_e mal_timer_get_valid_timers(const mal_timer_e **timers, uint8_t *size) {
    *timers = valid_timers;
    *size = sizeof(valid_timers) / sizeof(mal_timer_e);
    return MAL_ERROR_OK;
}
