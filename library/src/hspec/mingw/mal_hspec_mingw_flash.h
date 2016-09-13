/*
 * mal_hspec_mingw_flash.h
 *
 *  Created on: Sep 8, 2016
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

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_FLASH_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_FLASH_H_

#include "std/mal_error.h"
#include "std/mal_stdint.h"

typedef struct {
	uint32_t size;
} mal_hspec_mingw_flash_page_info_s;

typedef struct {
	mal_hspec_mingw_flash_page_info_s *pages;
	uint32_t page_count;
} mal_hspec_mingw_flash_info_s;

uint32_t mal_hspec_mingw_flash_get_page_count(void);

void mal_hspec_mingw_flash_set_flash(mal_hspec_mingw_flash_info_s *info);

uint32_t mal_hspec_mingw_flash_get_page_size(uint32_t page);

mal_error_e mal_hspec_mingw_flash_erase_page(uint32_t page);

mal_error_e mal_hspec_mingw_flash_write_uint8_values(uint64_t start_address, uint8_t *values, uint32_t count);

mal_error_e mal_hspec_mingw_flash_write_uint16_values(uint64_t start_address, uint16_t *values, uint32_t count);

mal_error_e mal_hspec_mingw_flash_write_uint32_values(uint64_t start_address, uint32_t *values, uint32_t count);

uint32_t mal_hspec_mingw_flash_get_page_from_address(uint64_t address);

uint64_t mal_hspec_mingw_flash_get_page_start_address(uint32_t page);

uint8_t mal_hspec_mingw_flash_read_uint8(uint64_t address);

int8_t mal_hspec_mingw_flash_read_int8(uint64_t address);

uint16_t mal_hspec_mingw_flash_read_uint16(uint64_t address);

int16_t	mal_hspec_mingw_flash_read_int16(uint64_t address);

uint32_t mal_hspec_mingw_flash_read_uint32(uint64_t address);

int32_t	mal_hspec_mingw_flash_read_int32(uint64_t address);

uint64_t mal_hspec_mingw_flash_read_uint64(uint64_t address);

int64_t	mal_hspec_mingw_flash_read_int64(uint64_t address);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_FLASH_H_ */
