/*
 * mal_hspec_mingw_timer.h
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

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_TIMER_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_TIMER_H_

#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_mingw_get_valid_timers(const mal_hspec_timer_e **timers, uint8_t *size);

mal_error_e mal_hspec_mingw_timer_init(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_callback_t callback);

mal_error_e mal_hspec_mingw_timer_free(mal_hspec_timer_e timer);

mal_error_e mal_hspec_mingw_timer_count_init(mal_hspec_timer_e timer, float frequency);

mal_error_e mal_hspec_mingw_timer_get_resolution(mal_hspec_timer_e timer, uint8_t *resolution);

mal_error_e mal_hspec_mingw_timer_get_count_frequency(mal_hspec_timer_e timer, float *frequency);

mal_error_e mal_hspec_mingw_timer_get_count(mal_hspec_timer_e timer, uint64_t *count);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_TIMER_H_ */
