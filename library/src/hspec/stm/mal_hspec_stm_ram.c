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

#include "ram/mal_ram.h"

MAL_DEFS_INLINE uint8_t mal_ram_read(unsigned int address) {
    uint8_t *byte_pointer = (uint8_t*)(long)address;
    return *byte_pointer;
}

MAL_DEFS_INLINE void mal_ram_write(unsigned int address, uint8_t value) {
    uint8_t *byte_pointer = (uint8_t*)(long)address;
    *byte_pointer = value;
}
