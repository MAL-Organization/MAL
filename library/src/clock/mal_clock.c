/*
 * mal_clock.c
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

#include "clock/mal_clock.h"
#include "std/mal_stdint.h"

void mal_clock_set_system_clock(uint64_t clock);

static uint64_t system_clock;

void mal_clock_set_system_clock(uint64_t clock) {
	system_clock = clock;
}

uint64_t mal_clock_get_system_clock(void) {
	return system_clock;
}
