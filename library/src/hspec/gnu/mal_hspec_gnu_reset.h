/*
 * Copyright (c) 2018 Olivier Allaire
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

#ifndef HSPEC_GNU_MAL_HSPEC_GNU_RESET_H_
#define HSPEC_GNU_MAL_HSPEC_GNU_RESET_H_

#include "std/mal_bool.h"
#include "reset/mal_reset.h"

bool mal_hspec_gnu_reset_get_request(void);

void mal_hspec_gnu_reset_clear_request(void);

void mal_hspec_gnu_reset_set_reset_source(mal_reset_source_e source);

#endif /* HSPEC_GNU_MAL_HSPEC_GNU_RESET_H_ */
