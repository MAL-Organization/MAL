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

#include "flash/mal_flash.h"
#include "std/mal_bool.h"
#include "hspec/stm/stm32f7/mal_hspec_stm32f7_flash.h"

uint32_t mal_flash_get_page_size(uint32_t page) {
    // Determine page size
    bool dual_bank = mal_hspec_stm32f7_flash_is_dual_boot();
    uint32_t small_page_size = 0x8000; // 32KB, 32768 bytes
    if (dual_bank) {
        small_page_size = 0x4000; // 16KB, 16384 bytes
    }
    uint32_t medium_page_size = 0x20000; // 128KB, 131072 bytes
    if (dual_bank) {
        medium_page_size = 0x10000; // 64KB, 65536 bytes
    }
    uint32_t large_page_size = 0x40000; // 256KB, 262144 bytes
    if (dual_bank) {
        large_page_size = 0x20000; // 128KB, 131072 bytes
    }
    // Return proper page size
    switch (mal_hspec_stm32f7_flash_get_sector(page)) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 12:
        case 13:
        case 14:
        case 15:
            return small_page_size;
        case 4:
        case 16:
            return medium_page_size;
        default:
            return large_page_size;
    }
}
