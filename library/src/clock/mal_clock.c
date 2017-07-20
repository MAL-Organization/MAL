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

static mal_hertz_t system_clock;

mal_error_e mal_clock_set_system_clock(const mal_system_clk_s *clk) {
    mal_error_e result = mal_clock_set_system_clock_unmanaged(clk);
    if (MAL_ERROR_OK != result) {
        return result;
    }
	system_clock = clk->frequency;
	return MAL_ERROR_OK;
}

mal_hertz_t mal_clock_get_system_clock(void) {
	return system_clock;
}

extern const mal_system_clk_s mal_target_system_clock;
mal_error_e mal_clock_initialise_system_clock(void) {
    return mal_clock_set_system_clock(&mal_target_system_clock);
}

extern mal_hertz_t mal_external_clk_freq;
mal_hertz_t mal_clock_get_external_clock_frequency(void) {
    return mal_external_clk_freq;
}
