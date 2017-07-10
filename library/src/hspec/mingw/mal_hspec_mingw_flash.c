/*
 * mal_hspec_mingw_flash.c
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

#include <string.h>

#include "mal_hspec_mingw_flash.h"
#include "std/mal_stdlib.h"

static uint8_t **flash_pages = NULL;
static mal_hspec_mingw_flash_info_s flash_info = {
	NULL,
	0
};

void mal_hspec_mingw_flash_set_flash(mal_hspec_mingw_flash_info_s *info) {
	// Free previous pages
	for (uint32_t i = 0; i < flash_info.page_count; i++) {
		free(flash_pages[i]);
	}
	free(flash_pages);
	// Free previous flash info
	free(flash_info.pages);
	// Set page count
	flash_info.page_count = info->page_count;
	flash_info.pages = (mal_hspec_mingw_flash_page_info_s*)malloc(sizeof(mal_hspec_mingw_flash_page_info_s) * info->page_count);
	// Allocate pages
	flash_pages = (uint8_t**)malloc(sizeof(uint8_t*) * info->page_count);
	for (uint32_t i = 0; i < flash_info.page_count; i++) {
		flash_pages[i] = (uint8_t*)malloc(info->pages[i].size);
		// Save flash info
		flash_info.pages[i].size = info->pages[i].size;
		// Erase page
		mal_flash_erase_page(i);
	}
}

uint32_t mal_flash_get_page_count(void) {
	return flash_info.page_count;
}

uint32_t mal_flash_get_page_size(uint32_t page) {
	if (page >= mal_flash_get_page_count()) {
		return 0;
	}
	return flash_info.pages[page].size;
}

mal_error_e mal_flash_erase_page(uint32_t page) {
	memset(flash_pages[page], 0xFF, flash_info.pages[page].size);
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_flash_write_uint8_values(uint64_t start_address, uint8_t *values, uint32_t count) {
	for (uint32_t i = 0; i < count; i++) {
		uint64_t current_address = start_address + i;
		uint32_t current_page = mal_flash_get_page_from_address(current_address);
		uint64_t address_offset = current_address - mal_flash_get_page_start_address(current_page);
		// Write byte
		flash_pages[current_page][address_offset] = values[i];
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_flash_write_uint16_values(unsigned int start_address, uint16_t *values, uint32_t count) {
	return mal_hspec_mingw_flash_write_uint8_values(start_address, (uint8_t*)values, count * sizeof(uint16_t));
}

mal_error_e mal_flash_write_uint32_values(unsigned int start_address, uint32_t *values, uint32_t count) {
	return mal_hspec_mingw_flash_write_uint8_values(start_address, (uint8_t*)values, count * sizeof(uint32_t));
}

uint32_t mal_flash_get_page_from_address(unsigned int address) {
	uint64_t current_start_address = 0;
	for (uint32_t i = 0; i < flash_info.page_count; i++) {
		// Compute current end address
		uint64_t current_end_address = current_start_address + mal_flash_get_page_size(i) - 1;
		// Check if we found the page
		if (address >= current_start_address && address <= current_end_address) {
			return i;
		}
		// Compute current start address for next page
		current_start_address += mal_flash_get_page_size(i);
	}
	return 0;
}

unsigned int mal_flash_get_page_start_address(uint32_t page) {
	uint64_t current_start_address = 0;
	for (uint32_t i = 0; i < page; i++) {
		current_start_address += mal_flash_get_page_size(i);
	}
	return current_start_address;
}

MAL_DEFS_INLINE uint8_t mal_flash_read_uint8(unsigned int address) {
	uint32_t page = mal_flash_get_page_from_address(address);
	uint64_t address_offset = address - mal_flash_get_page_start_address(page);
	return flash_pages[page][address_offset];
}

MAL_DEFS_INLINE int8_t mal_flash_read_int8(unsigned int address) {
	return (int8_t)mal_flash_read_uint8(address);
}

MAL_DEFS_INLINE uint16_t mal_flash_read_uint16(unsigned int address) {
	uint8_t byte_0 = mal_flash_read_uint8(address);
	uint8_t byte_1 = mal_flash_read_uint8(address + 1);
	uint16_t value = byte_0;
	value |= (uint16_t)byte_1 << 8;
	return value;
}

MAL_DEFS_INLINE int16_t mal_flash_read_int16(unsigned int address) {
	return (int16_t)mal_flash_read_uint16(address);
}

MAL_DEFS_INLINE uint32_t mal_flash_read_uint32(unsigned int address) {
	uint16_t word_0 = mal_flash_read_uint16(address);
	uint16_t word_1 = mal_flash_read_uint16(address + 2);
	uint32_t value = word_0;
	value |= (uint32_t)word_1 << 16;
	return value;
}

MAL_DEFS_INLINE int32_t mal_flash_read_int32(unsigned int address) {
	return (int32_t)mal_flash_read_uint32(address);
}

MAL_DEFS_INLINE uint64_t mal_flash_read_uint64(unsigned int address) {
	uint32_t word_0 = mal_flash_read_uint32(address);
	uint32_t word_1 = mal_flash_read_uint32(address + 4);
	uint64_t value = word_0;
	value |= (uint64_t)word_1 << 32;
	return value;
}

MAL_DEFS_INLINE uint8_t mal_flash_read_int64(unsigned int address) {
	return (int64_t)mal_flash_read_uint64(address);
}
