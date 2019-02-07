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

#include "mal_e3prom.h"
#include "flash/mal_flash.h"
#include "std/mal_bool.h"
#include "std/mal_stdlib.h"

/**
 * The value that should be read from key when read from an empty key value
 * space.
 */
#define MAL_E3PROM_EMPTY_KEY			0xFFFFFFFF
/**
 * The number of key value spaces to keep empty before declaring a sector full.
 * This is important to change the state of a sector.
 */
#define MAL_E3PROM_STATE_SECTOR_BUFFER	1

static mal_e3prom_state_e mal_e3prom_get_section_state(mal_e3prom_s *e3prom, mal_e3prom_section_e section);
static mal_error_e mal_e3prom_get_section_value(mal_e3prom_s *e3prom, mal_e3prom_section_e section, uint32_t key,
                                                uint32_t *value);
static mal_error_e mal_e3prom_write_section_value(mal_e3prom_s *e3prom, mal_e3prom_section_e section, uint32_t key,
                                                  uint32_t value, bool user_value);
static mal_error_e mal_e3prom_switch_active_sector(mal_e3prom_s *e3prom, bool skip_decommissioning, mal_e3prom_filter_t filter,
                                                   void *filter_handle);
static mal_error_e mal_e3prom_erase_section(mal_e3prom_s *e3prom, mal_e3prom_section_e section);
static mal_error_e mal_e3prom_common_write_value(mal_e3prom_s *e3prom, uint32_t key, uint32_t value);

mal_error_e mal_e3prom_init(mal_e3prom_init_s *init, mal_e3prom_s *e3prom) {
	mal_error_e result;
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
	mal_e3prom_state_e primary_state = mal_e3prom_get_section_state(e3prom, MAL_E3PROM_SECTION_PRIMARY);
	mal_e3prom_state_e secondary_state = mal_e3prom_get_section_state(e3prom, MAL_E3PROM_SECTION_SECONDARY);
	// If sections are in unknown state or initializing state, we erase them
	if (MAL_E3PROM_STATE_UNKNOWN == primary_state || MAL_E3PROM_STATE_INITIALIZING == primary_state) {
		result = mal_e3prom_erase_section(e3prom, MAL_E3PROM_SECTION_PRIMARY);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		primary_state = MAL_E3PROM_STATE_ERASED;
	}
	if (MAL_E3PROM_STATE_UNKNOWN == secondary_state || MAL_E3PROM_STATE_INITIALIZING == secondary_state) {
		result = mal_e3prom_erase_section(e3prom, MAL_E3PROM_SECTION_SECONDARY);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		secondary_state = MAL_E3PROM_STATE_ERASED;
	}
	// Determine the active sector
	// The first possibility is that both sections are erased. In that case, we
	// simply choose the primary as active.
	if (MAL_E3PROM_STATE_ERASED == primary_state && MAL_E3PROM_STATE_ERASED == secondary_state) {
		e3prom->active_section = MAL_E3PROM_SECTION_PRIMARY;
		result = mal_e3prom_write_section_value(e3prom, e3prom->active_section, MAL_E3PROM_STATE_KEY,
                                                MAL_E3PROM_STATE_ACTIVE, false);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	}
	// The next possibility is that only the primary is active. If, for some
	// reason, we have 2 active partitions, we will favor the primary.
	else if (MAL_E3PROM_STATE_ACTIVE == primary_state) {
		e3prom->active_section = MAL_E3PROM_SECTION_PRIMARY;
	}
	// The next possibility is that only the secondary is active.
	else if (MAL_E3PROM_STATE_ACTIVE == secondary_state) {
		e3prom->active_section = MAL_E3PROM_SECTION_SECONDARY;
	}
	// If we have the primary section decommossioned, we will switch
	// sections.
	else if (MAL_E3PROM_STATE_DECOMMISSIONED == primary_state) {
		e3prom->active_section = MAL_E3PROM_SECTION_PRIMARY;
		result = mal_e3prom_switch_active_sector(e3prom, true, NULL, NULL);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	}
	// If we have the secondary section decommossioned, we will switch
	// sections.
	else if (MAL_E3PROM_STATE_DECOMMISSIONED == secondary_state) {
		e3prom->active_section = MAL_E3PROM_SECTION_SECONDARY;
		result = mal_e3prom_switch_active_sector(e3prom, true, NULL, NULL);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	} else {
		// Should never get here
		return MAL_ERROR_INIT_FAILED;
	}

	return MAL_ERROR_OK;
}

static mal_e3prom_state_e mal_e3prom_get_section_state(mal_e3prom_s *e3prom, mal_e3prom_section_e section) {
	// Read value
	mal_error_e result;
	uint32_t value;
	result = mal_e3prom_get_section_value(e3prom, section, MAL_E3PROM_STATE_KEY, &value);
	if (MAL_ERROR_OK == result) {
		return value;
	}
	return MAL_E3PROM_STATE_UNKNOWN;
}

mal_error_e mal_e3prom_get_value(mal_e3prom_s *e3prom, uint32_t key, uint32_t *value) {
	return mal_e3prom_get_section_value(e3prom, e3prom->active_section, key, value);
}

static mal_error_e mal_e3prom_get_section_value(mal_e3prom_s *e3prom, mal_e3prom_section_e section, uint32_t key,
                                                uint32_t *value) {
	// Get first key match reading backwards in section
	uint64_t key_address;
	uint64_t start_address = mal_flash_get_page_start_address(e3prom->sections[section].start_page);
	uint64_t last_address = e3prom->sections[section].last_address;
	uint32_t step = sizeof(uint32_t) * 2;
	for (key_address = last_address;
		 key_address >= start_address && key_address <= last_address;
		 key_address -= step) {
		uint32_t current_key = mal_flash_read_uint32((unsigned int)key_address);
		if (current_key == key) {
			uint64_t value_address = key_address + sizeof(uint32_t);
			*value = mal_flash_read_uint32((unsigned int)value_address);
			return MAL_ERROR_OK;
		}
	}
	return MAL_ERROR_NOT_FOUND;
}

static mal_error_e mal_e3prom_erase_section(mal_e3prom_s *e3prom, mal_e3prom_section_e section) {
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
	return mal_e3prom_write_section_value(e3prom, section, MAL_E3PROM_STATE_KEY, MAL_E3PROM_STATE_ERASED, false);
}

static mal_error_e mal_e3prom_common_write_value(mal_e3prom_s *e3prom, uint32_t key, uint32_t value) {
	// Users cannot write state
	if (MAL_E3PROM_STATE_KEY == key) {
		return MAL_ERROR_OPERATION_INVALID;
	}
	// Try to write value to active sector
	return mal_e3prom_write_section_value(e3prom, e3prom->active_section, key, value, true);
}

mal_error_e mal_e3prom_write_value(mal_e3prom_s *e3prom, uint32_t key, uint32_t value) {
	mal_error_e result;
	// Users cannot write state
	if (MAL_E3PROM_STATE_KEY == key) {
		return MAL_ERROR_OPERATION_INVALID;
	}
	// Try to write value to active sector
	result = mal_e3prom_common_write_value(e3prom, key, value);
	if (MAL_ERROR_OK == result) {
		return result;
	}
	// Make sure the write did not fail in an unexpected way
	if (MAL_ERROR_FULL != result) {
		return result;
	}
	// When we get here, we need to switch active sector
	result = mal_e3prom_switch_active_sector(e3prom, false, NULL, NULL);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Write value to new sector
	return mal_e3prom_write_section_value(e3prom, e3prom->active_section, key, value, true);
}

static mal_error_e mal_e3prom_write_section_value(mal_e3prom_s *e3prom, mal_e3prom_section_e section, uint32_t key,
                                                  uint32_t value, bool user_value) {
	mal_error_e result;
	// Key cannot be empty value
	if (MAL_E3PROM_EMPTY_KEY == key) {
		return MAL_ERROR_OPERATION_INVALID;
	}
	// Find the next available space
	uint64_t key_address;
	uint32_t step = sizeof(uint32_t) * 2;
	uint64_t start_address = mal_flash_get_page_start_address(e3prom->sections[section].start_page);
	uint64_t last_address = e3prom->sections[section].last_address;
	if (user_value) {
		last_address -= step * MAL_E3PROM_STATE_SECTOR_BUFFER;
	}
	for (key_address = start_address; key_address <= last_address; key_address += step) {
		uint32_t current_key = mal_flash_read_uint32((unsigned int)key_address);
		if (MAL_E3PROM_EMPTY_KEY == current_key) {
			break;
		}
	}
	// Make sure an empty key was found
	if (key_address > last_address) {
		return MAL_ERROR_FULL;
	}
	// Write key
	result = mal_flash_write_uint32_values((unsigned int)key_address, &key, 1);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Write value
	uint64_t value_address = key_address + sizeof(uint32_t);
	return mal_flash_write_uint32_values((unsigned int)value_address, &value, 1);
}

static mal_error_e mal_e3prom_switch_active_sector(mal_e3prom_s *e3prom, bool skip_decommissioning,
                                                   mal_e3prom_filter_t filter, void *filter_handle) {
	mal_error_e result;
	// Make the active section decommissioned.
	if (!skip_decommissioning) {
		result = mal_e3prom_write_section_value(e3prom, e3prom->active_section, MAL_E3PROM_STATE_KEY,
                                                MAL_E3PROM_STATE_DECOMMISSIONED, false);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	}
	// Switch active sector pointer
	mal_e3prom_section_e previous_section = e3prom->active_section;
	if (MAL_E3PROM_SECTION_PRIMARY == e3prom->active_section) {
		e3prom->active_section = MAL_E3PROM_SECTION_SECONDARY;
	} else {
		e3prom->active_section = MAL_E3PROM_SECTION_PRIMARY;
	}
	// Make sure current state of new sector is erased
	mal_e3prom_state_e state = mal_e3prom_get_section_state(e3prom, e3prom->active_section);
	if (MAL_E3PROM_STATE_ERASED != state) {
		result = mal_e3prom_erase_section(e3prom, e3prom->active_section);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	}
	// Change state to initializing
	result = mal_e3prom_write_section_value(e3prom, e3prom->active_section, MAL_E3PROM_STATE_KEY,
                                            MAL_E3PROM_STATE_INITIALIZING, false);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Copy values from previous sector
	uint64_t previous_key_address;
	uint32_t step = sizeof(uint32_t) * 2;
	uint64_t previous_start_address = mal_flash_get_page_start_address(e3prom->sections[previous_section].start_page);
	uint64_t previous_last_address = e3prom->sections[previous_section].last_address - (step * MAL_E3PROM_STATE_SECTOR_BUFFER);
	uint64_t active_start_address = mal_flash_get_page_start_address(e3prom->sections[e3prom->active_section].start_page);
	uint64_t active_last_address = e3prom->sections[e3prom->active_section].last_address - (step * MAL_E3PROM_STATE_SECTOR_BUFFER);
	for (previous_key_address = previous_last_address;
		 previous_key_address >= previous_start_address && previous_key_address <= previous_last_address;
		 previous_key_address -= step) {
		uint32_t previous_key = mal_flash_read_uint32((unsigned int)previous_key_address);
		// Check if key value should be transferred
		if (MAL_E3PROM_EMPTY_KEY == previous_key || MAL_E3PROM_STATE_KEY == previous_key) {
			continue;
		}
		// Check if this value should be filtered
		uint64_t previous_value_address = previous_key_address + sizeof(uint32_t);
		uint32_t previous_value = mal_flash_read_uint32((unsigned int)previous_value_address);
		if (NULL != filter) {
			if (!filter(filter_handle, e3prom, previous_key, previous_value)) {
				continue;
			}
		}
		// Make sure this key was not already transferred
		bool key_found = false;
		uint64_t active_key_address;
		for (active_key_address = active_start_address; active_key_address <= active_last_address; active_key_address += step) {
			uint32_t active_key = mal_flash_read_uint32((unsigned int)active_key_address);
			// Check if we reached the end
			if (MAL_E3PROM_EMPTY_KEY == active_key) {
				break;
			}
			// Check if key matches
			if (previous_key == active_key) {
				key_found = true;
				break;
			}
		}
		if (key_found) {
			continue;
		}
		// Value was not transferred, write key value
		result = mal_e3prom_write_section_value(e3prom, e3prom->active_section, previous_key, previous_value, true);
		if (MAL_ERROR_OK != result) {
			return result;
		}
	}
	// Change state to active
	return mal_e3prom_write_section_value(e3prom, e3prom->active_section, MAL_E3PROM_STATE_KEY, MAL_E3PROM_STATE_ACTIVE,
                                          false);
}

mal_error_e mal_e3prom_filter(mal_e3prom_s *e3prom, mal_e3prom_filter_t filter, void *handle) {
    return mal_e3prom_switch_active_sector(e3prom, false, filter, handle);
}

mal_error_e mal_async_e3prom_init(mal_async_e3prom_init_s *init, mal_async_e3prom_s *async_e3prom) {
	return mal_e3prom_init(&init->e3prom_init, &async_e3prom->e3prom);
}

mal_error_e mal_async_e3prom_get_value(mal_async_e3prom_s *async_e3prom, uint32_t key, uint32_t *value) {
	return mal_e3prom_get_value(&async_e3prom->e3prom, key, value);
}

mal_error_e mal_async_e3prom_write_value(mal_async_e3prom_s *async_e3prom, uint32_t key, uint32_t value) {
	mal_error_e result;
	// Users cannot write state
	if (MAL_E3PROM_STATE_KEY == key) {
		return MAL_ERROR_OPERATION_INVALID;
	}
	// Try to write value to active sector
	result = mal_e3prom_common_write_value(&async_e3prom->e3prom, key, value);
	if (MAL_ERROR_OK == result) {
		return result;
	}
	// Make sure the write did not fail in an unexpected way
	if (MAL_ERROR_FULL != result) {
		return result;
	}
}
