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

#include "mal_hspec_gnu_ram.h"
#include "std/mal_stdlib.h"

unsigned int mal_hspec_gnu_ram_stack_start = 0;
unsigned int mal_hspec_gnu_ram_stack_end = 0;
unsigned int mal_hspec_gnu_ram_stack_pointer_address = 0;
uint8_t *mal_hspec_gnu_ram_simulated_ram = NULL;

unsigned int mal_ram_get_stack_start_address(void) {
    return mal_hspec_gnu_ram_stack_start;
}

unsigned int mal_ram_get_stack_end_address(void) {
    return mal_hspec_gnu_ram_stack_end;
}

unsigned int mal_ram_get_stack_pointer_address(void) {
    return mal_hspec_gnu_ram_stack_pointer_address;
}

MAL_DEFS_INLINE uint8_t mal_ram_read(unsigned int address) {
    address = address - mal_hspec_gnu_ram_stack_start;
    return mal_hspec_gnu_ram_simulated_ram[address];
}

MAL_DEFS_INLINE void mal_ram_write(unsigned int address, uint8_t value) {
    address = address - mal_hspec_gnu_ram_stack_start;
    mal_hspec_gnu_ram_simulated_ram[address] = value;
}
