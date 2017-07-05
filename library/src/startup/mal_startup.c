/*
 * mal_startup.c
 *
 *  Created on: May 16, 2015
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

#include "startup/mal_startup.h"
#include "std/mal_defs.h"

void mal_timer_states_init(void);
void mal_reset_init(void);
int main(void);

void mal_startup_top_level(void) {
	// Hardware specific startup
    mal_startup_hardware();

	// Initialize library components
	mal_timer_states_init();
	mal_reset_init();
}

void MAL_DEFS_NO_RETURN mal_startup(void) {

    mal_startup_top_level();

	main();

	while (1);

}
