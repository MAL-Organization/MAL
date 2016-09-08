/*
 * mal_e3prom.c
 *
 *  Created on: Sep 6, 2016
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

#include "mal_e3prom.h"
#include "flash/mal_flash.h"

#define EMPTY_KEY	0xFFFFFFFF

static mal_e3prom_state_e get_section_state(mal_e3prom_s *e3prom, mal_e3prom_section_e section);
static mal_error_e get_section_value(mal_e3prom_s *e3prom, mal_e3prom_section_e section, uint32_t key, uint32_t *value);
static mal_error_e write_section_value(mal_e3prom_s *e3prom, mal_e3prom_section_e section, uint32_t key, uint32_t value);

void mal_e3prom_init(mal_e3prom_init_s *init, mal_e3prom_s *e3prom) {
	// Store page data
	e3prom->sections[MAL_E3PROM_SECTION_PRIMARY].start_page = init->primary_start_page;
	e3prom->sections[MAL_E3PROM_SECTION_SECONDARY].start_page = init->secondary_start_page;
	e3prom->sections[MAL_E3PROM_SECTION_PRIMARY].page_count = init->primary_page_count;
	e3prom->sections[MAL_E3PROM_SECTION_SECONDARY].page_count = init->secondary_page_count;
	// Compute last addresses. They are the last address of each sections.
	uint32_t i;
	uint32_t last_address;
	// Primary state
	last_address = mal_flash_get_page_start_address(init->primary_start_page);
	for (i = 0; i < init->primary_page_count; i++) {
		uint32_t page = init->primary_start_page + i;
		uint32_t size = mal_flash_get_page_size(page);
		last_address += size;
	}
	// We track back one key value space
	last_address -= sizeof(uint32_t) * 2;
	e3prom->sections[MAL_E3PROM_SECTION_PRIMARY].last_address = last_address;
	// Secondary state
	last_address = mal_flash_get_page_start_address(init->secondary_start_page);
	for (i = 0; i < init->secondary_page_count; i++) {
		uint32_t page = init->secondary_start_page + i;
		uint32_t size = mal_flash_get_page_size(page);
		last_address += size;
	}
	// We track back one key value space
	last_address -= sizeof(uint32_t) * 2;
	e3prom->sections[MAL_E3PROM_SECTION_SECONDARY].last_address = last_address;
	// Get state of each sections
	mal_e3prom_state_e primary_state = get_section_state(e3prom, MAL_E3PROM_SECTION_PRIMARY);
	mal_e3prom_state_e secondary_state = get_section_state(e3prom, MAL_E3PROM_SECTION_SECONDARY);
	// If sections are in unknown state, we erase them
	if (MAL_E3PROM_STATE_UNKNOWN == primary_state) {

	}
}

static mal_e3prom_state_e get_section_state(mal_e3prom_s *e3prom, mal_e3prom_section_e section) {
	// Read value
	mal_error_e result;
	uint32_t value;
	result = get_section_value(e3prom, section, MAL_E3PROM_STATE_KEY, &value);
	if (MAL_ERROR_OK == result) {
		return value;
	}
	return MAL_E3PROM_STATE_UNKNOWN;
}

mal_error_e mal_e3prom_get_value(mal_e3prom_s *e3prom, uint32_t key, uint32_t *value) {
	return get_section_value(e3prom, e3prom->active_section, key, value);
}

static mal_error_e get_section_value(mal_e3prom_s *e3prom, mal_e3prom_section_e section, uint32_t key, uint32_t *value) {
	// Get first key match reading backwards in section
	uint64_t key_address;
	uint64_t start_address = mal_flash_get_page_start_address(e3prom->sections[section].start_page);
	uint64_t last_address = e3prom->sections[section].last_address;
	uint32_t step = sizeof(uint32_t) * 2;
	for (key_address = last_address; key_address >= start_address; key_address -= step) {
		uint32_t current_key = MAL_FLASH_READ_UINT32(key_address);
		if (current_key == key) {
			uint64_t value_address = key_address + sizeof(uint32_t);
			*value = MAL_FLASH_READ_UINT32(value_address);
			return MAL_ERROR_OK;
		}
	}
	return MAL_ERROR_NOT_FOUND;
}

static mal_error_e erase_section(mal_e3prom_s *e3prom, mal_e3prom_section_e section) {
	mal_error_e result;
	// Erase every page
	uint32_t i;
	for (i = 0; i < e3prom->sections[section].page_count; i++) {
		result = mal_flash_erase_page(e3prom->sections[section].start_page + i);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	}
	// Write state erased
}

mal_error_e mal_e3prom_write_value(mal_e3prom_s *e3prom, uint32_t key, uint32_t value) {
	return write_section_value(e3prom, e3prom->active_section, key, value);
}

static mal_error_e write_section_value(mal_e3prom_s *e3prom, mal_e3prom_section_e section, uint32_t key, uint32_t value) {
	// Find the next available space
	uint64_t key_address;
	uint64_t start_address = mal_flash_get_page_start_address(e3prom->sections[section].start_page);
	uint64_t last_address = e3prom->sections[section].last_address;
	uint32_t step = sizeof(uint32_t) * 2;
	for (key_address = start_address; key_address <= last_address; key_address += step) {
		uint32_t current_key = MAL_FLASH_READ_UINT32(key_address);
		if (EMPTY_KEY == current_key) {
			break;
		}
	}
	// Make sure an empty key was found
}
