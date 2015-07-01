/*
 * mal_clock.h
 *
 *  Created on: May 28, 2015
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
 *
 * This file is part of MAL.
 *
 * MAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CLOCK_MAL_CLOCK_H_
#define CLOCK_MAL_CLOCK_H_

#include "std/mal_stdint.h"
#include "hspec/mal_hspec.h"

#define mal_clock_get_timer_input_clk(timer, clock) mal_hspec_get_timer_input_clk(timer, clock)

uint64_t mal_clock_get_system_clock(void);

#endif /* CLOCK_MAL_CLOCK_H_ */
