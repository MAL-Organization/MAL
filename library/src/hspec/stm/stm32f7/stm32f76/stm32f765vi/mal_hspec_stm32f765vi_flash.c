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

#include "hspec/stm/stm32f7/mal_hspec_stm32f7_flash.h"

uint32_t mal_hspec_stm32f7_flash_get_sector(uint32_t page) {
    return page;
}

uint32_t mal_flash_get_page_count(void) {
    if (mal_hspec_stm32f7_flash_is_dual_boot()) {
        return 24;
    }
    return 12;
}
