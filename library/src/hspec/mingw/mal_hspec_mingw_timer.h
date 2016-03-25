/*
 * mal_hspec_mingw_timer.h
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_TIMER_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_TIMER_H_

#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_mingw_get_valid_timers(const mal_hspec_timer_e **timers, uint8_t *size);

mal_error_e mal_hspec_mingw_timer_init(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_callback_t callback);

mal_error_e mal_hspec_mingw_timer_free(mal_hspec_timer_e timer);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_TIMER_H_ */
