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

#ifndef HSPEC_GNU_MAL_HSPEC_GNU_FLASH_H_
#define HSPEC_GNU_MAL_HSPEC_GNU_FLASH_H_

#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "flash/mal_flash.h"

typedef struct {
	uint32_t size;
} mal_hspec_gnu_flash_page_info_s;

typedef struct {
	mal_hspec_gnu_flash_page_info_s *pages;
	uint32_t page_count;
} mal_hspec_gnu_flash_info_s;

void mal_hspec_gnu_flash_set_flash(mal_hspec_gnu_flash_info_s *info);

mal_error_e mal_hspec_gnu_flash_write_uint8_values(uint64_t start_address, uint8_t *values, uint32_t count);

#endif /* HSPEC_GNU_MAL_HSPEC_GNU_FLASH_H_ */
