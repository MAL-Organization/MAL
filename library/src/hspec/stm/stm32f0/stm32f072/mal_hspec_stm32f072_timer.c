/*
 * mal_hspec_stm32f072_timer.c
 *
 *  Created on: Sep 6, 2016
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

#include "hspec/stm/stm32f0/mal_hspec_stm32f0_timer.h"

static const mal_timer_e valid_timers[] = {
	MAL_TIMER_1,
	MAL_TIMER_2,
	MAL_TIMER_3,
	MAL_TIMER_6,
	MAL_TIMER_7,
	MAL_TIMER_14,
	MAL_TIMER_15,
	MAL_TIMER_16,
	MAL_TIMER_17
};

mal_error_e mal_timer_get_valid_timers(const mal_timer_e **timers, uint8_t *size) {
	*timers = valid_timers;
	*size = sizeof(valid_timers) / sizeof(mal_timer_e);
	return MAL_ERROR_OK;
}
