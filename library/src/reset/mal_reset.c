/*
 * mal_reset.c
 *
 *  Created on: Jul 27, 2016
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

#include "reset/mal_reset.h"

/**
 * This will be called at startup only once.
 */
void mal_reset_init(void);

static mal_hspec_reset_source_e reset_source;

void mal_reset_init(void) {
	reset_source = mal_hspec_reset_handle_reset_source();
}

mal_hspec_reset_source_e mal_reset_get_source(void) {
	return reset_source;
}
