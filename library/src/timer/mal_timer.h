/*
 * mal_timer.h
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

#ifndef TIMER_MAL_TIMER_H_
#define TIMER_MAL_TIMER_H_

#include "hspec/mal_hspec_cmn.h"
#include "std/mal_error.h"
#include "std/mal_stdint.h"

// error is in uHz
mal_error_e mal_timer_init_tick(mal_hspec_timer_e timer, float frequency, float delta, mal_hspec_timer_e *handle);

uint64_t mal_timer_get_tick(mal_hspec_timer_e handle);

mal_error_e mal_timer_free(mal_hspec_timer_e timer);

#endif /* TIMER_MAL_TIMER_H_ */
