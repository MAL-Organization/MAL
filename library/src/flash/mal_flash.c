/*
 * mal_flash.c
 *
 *  Created on: Sep 2, 2016
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

#include "mal_flash.h"

mal_error_e mal_flash_is_page_valid(uint32_t page) {
    if (page >= mal_flash_get_page_count()) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_flash_safe_erase_page(uint32_t page) {
	mal_error_e result;
	// Make sure flash page is valid
	result = mal_flash_is_page_valid(page);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Erase page
	return mal_flash_erase_page(page);
}

