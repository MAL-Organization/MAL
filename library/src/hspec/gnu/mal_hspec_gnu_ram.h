/*
 * Copyright (c) 2019 Olivier Allaire
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

#ifndef MAL_MAL_HSPEC_GNU_RAM_H
#define MAL_MAL_HSPEC_GNU_RAM_H

#include "ram/mal_ram.h"

extern unsigned int mal_hspec_gnu_ram_stack_start;
extern unsigned int mal_hspec_gnu_ram_stack_end;
extern unsigned int mal_hspec_gnu_ram_stack_pointer_address;
extern uint8_t *mal_hspec_gnu_ram_simulated_ram;

#endif //MAL_MAL_HSPEC_GNU_RAM_H
