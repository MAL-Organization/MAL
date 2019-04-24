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
static uint64_t mal_e3prom_get_previous_section_last_address(mal_e3prom_s *e3prom);
static mal_error_e mal_e3prom_switch_active_sector_step(mal_e3prom_s *e3prom, uint64_t step_burst_size,
														uint64_t *previous_key_address, mal_e3prom_filter_t filter,
														void *filter_handle);
static mal_error_e mal_async_e3prom_save_pending_write(mal_async_e3prom_s *async_e3prom, uint32_t key, uint32_t value);
static mal_e3prom_section_e mal_e3prom_get_previous_section(mal_e3prom_s *e3prom);
static mal_error_e mal_async_e3prom_handle_pending_writes(mal_async_e3prom_s *async_e3prom);

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

static mal_error_e mal_e3prom_start_active_sector_switch(mal_e3prom_s *e3prom, bool skip_decommissioning) {
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
	return mal_e3prom_write_section_value(e3prom, e3prom->active_section, MAL_E3PROM_STATE_KEY,
			                              MAL_E3PROM_STATE_INITIALIZING, false);
}

static mal_e3prom_section_e mal_e3prom_get_previous_section(mal_e3prom_s *e3prom) {
	mal_e3prom_section_e previous_section = MAL_E3PROM_SECTION_PRIMARY;
	if (MAL_E3PROM_SECTION_PRIMARY == e3prom->active_section) {
		previous_section = MAL_E3PROM_SECTION_SECONDARY;
	}
	return previous_section;
}

static uint64_t mal_e3prom_get_previous_section_last_address(mal_e3prom_s *e3prom) {
	// Determine previous section
	mal_e3prom_section_e previous_section = mal_e3prom_get_previous_section(e3prom);
	uint32_t step = sizeof(uint32_t) * 2;
	return e3prom->sections[previous_section].last_address - (step * MAL_E3PROM_STATE_SECTOR_BUFFER);
}

static mal_error_e mal_e3prom_switch_active_sector_step(mal_e3prom_s *e3prom, uint64_t step_burst_size,
		                                                uint64_t *previous_key_address, mal_e3prom_filter_t filter,
		                                                void *filter_handle) {
	mal_error_e result;
	// Copy values from previous sector
	mal_e3prom_section_e previous_section = mal_e3prom_get_previous_section(e3prom);
	uint64_t step_counter = 0;
	uint32_t step = sizeof(uint32_t) * 2;
	uint64_t previous_start_address = mal_flash_get_page_start_address(e3prom->sections[previous_section].start_page);
	uint64_t previous_last_address = mal_e3prom_get_previous_section_last_address(e3prom);
	uint64_t active_start_address = mal_flash_get_page_start_address(e3prom->sections[e3prom->active_section].start_page);
	uint64_t active_last_address = e3prom->sections[e3prom->active_section].last_address - (step * MAL_E3PROM_STATE_SECTOR_BUFFER);
	for (; (*previous_key_address >= previous_start_address) && (*previous_key_address <= previous_last_address);
		 *previous_key_address -= step) {
		step_counter++;
		uint32_t previous_key = mal_flash_read_uint32((unsigned int)*previous_key_address);
		// Check if key value should be transferred
		if ((MAL_E3PROM_EMPTY_KEY == previous_key) || (MAL_E3PROM_STATE_KEY == previous_key)) {
			if (step_counter >= step_burst_size) {
				return MAL_ERROR_IN_PROGRESS;
			}
			continue;
		}
		// Check if this value should be filtered
		uint64_t previous_value_address = *previous_key_address + sizeof(uint32_t);
		uint32_t previous_value = mal_flash_read_uint32((unsigned int)previous_value_address);
		if (NULL != filter) {
			if (!filter(filter_handle, e3prom, previous_key, previous_value)) {
				if (step_counter >= step_burst_size) {
					return MAL_ERROR_IN_PROGRESS;
				}
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
			if (step_counter >= step_burst_size) {
				return MAL_ERROR_IN_PROGRESS;
			}
			continue;
		}
		// Value was not transferred, write key value
		result = mal_e3prom_write_section_value(e3prom, e3prom->active_section, previous_key, previous_value, true);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		// Check burst size
		if (step_counter >= step_burst_size) {
			return MAL_ERROR_IN_PROGRESS;
		}
	}
	// Change state to active
	return mal_e3prom_write_section_value(e3prom, e3prom->active_section, MAL_E3PROM_STATE_KEY, MAL_E3PROM_STATE_ACTIVE,
										  false);
}

static mal_error_e mal_e3prom_switch_active_sector(mal_e3prom_s *e3prom, bool skip_decommissioning,
                                                   mal_e3prom_filter_t filter, void *filter_handle) {
	mal_error_e result;
	// Make the active section decommissioned.
	result = mal_e3prom_start_active_sector_switch(e3prom, skip_decommissioning);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	uint64_t previous_key_address = mal_e3prom_get_previous_section_last_address(e3prom);
	return mal_e3prom_switch_active_sector_step(e3prom, UINT64_MAX, &previous_key_address, filter, filter_handle);
}

mal_error_e mal_e3prom_filter(mal_e3prom_s *e3prom, mal_e3prom_filter_t filter, void *handle) {
    return mal_e3prom_switch_active_sector(e3prom, false, filter, handle);
}

mal_error_e mal_e3prom_search(mal_e3prom_s *e3prom, mal_e3prom_search_delegate_t search_delegate, void *handle) {
	uint64_t key_address;
	uint64_t start_address = mal_flash_get_page_start_address(e3prom->sections[e3prom->active_section].start_page);
	uint64_t last_address = e3prom->sections[e3prom->active_section].last_address;
	uint32_t step = sizeof(uint32_t) * 2;
	for (key_address = last_address;
		 key_address >= start_address && key_address <= last_address;
		 key_address -= step) {
		uint32_t key = mal_flash_read_uint32((unsigned int)key_address);
		if (MAL_E3PROM_EMPTY_KEY == key) {
			continue;
		}
		uint64_t value_address = key_address + sizeof(uint32_t);
		uint32_t value = mal_flash_read_uint32((unsigned int)value_address);
		mal_e3prom_search_result_e result = search_delegate(handle, e3prom, key, value);
		if (MAL_E3PROM_SEARCH_RESULT_FOUND == result) {
			return MAL_ERROR_OK;
		}
	}
	return MAL_ERROR_NOT_FOUND;
}

mal_error_e mal_async_e3prom_init(mal_async_e3prom_init_s *init, mal_async_e3prom_s *async_e3prom) {
	// Initialise async sector switch variables
	async_e3prom->burst_size = init->burst_size;
	async_e3prom->switch_in_progress = false;
	async_e3prom->active_filter = NULL;
	async_e3prom->active_filter_complete = NULL;
	async_e3prom->active_filter_handle = NULL;
	// Initialise pending write pool
	mal_pool_init(init->pending_write_object_array, init->pending_write_array_size, (uint8_t*)init->pending_write_array,
			      sizeof(mal_async_e3prom_pending_write_s), &async_e3prom->pending_writes_pool);
	// Initialise pending filter pool
	mal_pool_init(init->pending_filter_object_array, init->pending_filter_array_size,
			      (uint8_t*)init->pending_filter_array, sizeof(mal_async_e3prom_pending_filter_s),
			      &async_e3prom->pending_filters_pool);
	// Initialise E3PROM
	return mal_e3prom_init(&init->e3prom_init, &async_e3prom->e3prom);
}

mal_error_e mal_async_e3prom_get_value(mal_async_e3prom_s *async_e3prom, uint32_t key, uint32_t *value) {
	// Check if a switch is in progress
	if (!async_e3prom->switch_in_progress) {
		return mal_e3prom_get_value(&async_e3prom->e3prom, key, value);
	}
	// Check pending writes
	uint64_t index;
	mal_async_e3prom_pending_write_s *pending_write;
	MAL_POOL_FOR_EACH(&async_e3prom->pending_writes_pool, index, pending_write) {
		if (pending_write->key == key) {
			*value = pending_write->value;
			return MAL_ERROR_OK;
		}
	}
	// Read in previous sector
	mal_e3prom_section_e previous_section = mal_e3prom_get_previous_section(&async_e3prom->e3prom);
	return mal_e3prom_get_section_value(&async_e3prom->e3prom, previous_section, key, value);
}

static mal_error_e mal_async_e3prom_save_pending_write(mal_async_e3prom_s *async_e3prom, uint32_t key, uint32_t value) {
	mal_error_e result;
	// Make sure key is not already pending
	uint64_t index;
	mal_async_e3prom_pending_write_s *pending_write;
	MAL_POOL_FOR_EACH(&async_e3prom->pending_writes_pool, index, pending_write) {
		if (pending_write->key == key) {
			pending_write->value = value;
			return MAL_ERROR_OK;
		}
	}
	// Save pending write
	result = mal_pool_allocate(&async_e3prom->pending_writes_pool, (void**)&pending_write);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	pending_write->key = key;
	pending_write->value = value;
	return MAL_ERROR_OK;
}

mal_error_e mal_async_e3prom_write_value(mal_async_e3prom_s *async_e3prom, uint32_t key, uint32_t value) {
	mal_error_e result;
	// Users cannot write state
	if (MAL_E3PROM_STATE_KEY == key) {
		return MAL_ERROR_OPERATION_INVALID;
	}
	// Make sure a sector switch is not in progress
	if (async_e3prom->switch_in_progress) {
		return mal_async_e3prom_save_pending_write(async_e3prom, key, value);
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
	// Save write as pending
	result = mal_async_e3prom_save_pending_write(async_e3prom, key, value);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Start sector switch
	result = mal_e3prom_start_active_sector_switch(&async_e3prom->e3prom, false);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	async_e3prom->secsw_previous_key_address = mal_e3prom_get_previous_section_last_address(&async_e3prom->e3prom);
	result = mal_e3prom_switch_active_sector_step(&async_e3prom->e3prom, async_e3prom->burst_size,
												   &async_e3prom->secsw_previous_key_address, NULL, NULL);
	if (MAL_ERROR_IN_PROGRESS == result) {
		async_e3prom->switch_in_progress = true;
		result = MAL_ERROR_OK;
	} else if (MAL_ERROR_OK == result) {
		// We can do pending write
		result = mal_async_e3prom_handle_pending_writes(async_e3prom);
	}
	return result;
}

static mal_error_e mal_async_e3prom_handle_pending_writes(mal_async_e3prom_s *async_e3prom) {
	mal_error_e result = MAL_ERROR_OK;
	mal_error_e step_result;
	uint64_t index;
	mal_async_e3prom_pending_write_s *pending_write;
	MAL_POOL_FOR_EACH(&async_e3prom->pending_writes_pool, index, pending_write) {
		step_result = mal_async_e3prom_write_value(async_e3prom, pending_write->key, pending_write->value);
		if (MAL_ERROR_OK != step_result) {
			result = step_result;
		}
	}
	mal_pool_flush(&async_e3prom->pending_writes_pool);
	return result;
}

mal_error_e mal_async_e3prom_process(mal_async_e3prom_s *async_e3prom) {
	mal_error_e result = MAL_ERROR_OK;
	mal_error_e step_result;
	// Handle async switches
	if (async_e3prom->switch_in_progress) {
		// Execute burst
		result = mal_e3prom_switch_active_sector_step(&async_e3prom->e3prom,
				                                      async_e3prom->burst_size,
													   &async_e3prom->secsw_previous_key_address,
													   async_e3prom->active_filter,
													   async_e3prom->active_filter_handle);
		// Check if transfer is complete
		if (MAL_ERROR_OK == result) {
			// Execute complete callback
			mal_async_e3prom_pending_filter_s *next_pending_filter = NULL;
			if (NULL != async_e3prom->active_filter_complete) {
				async_e3prom->active_filter_complete(async_e3prom->active_filter_handle, async_e3prom);
				// Remove filter
				uint64_t index;
				mal_async_e3prom_pending_filter_s *current_pending_filter;
				MAL_POOL_FOR_EACH(&async_e3prom->pending_filters_pool, index, current_pending_filter) {
					if ((current_pending_filter->filter == async_e3prom->active_filter) &&
					    (current_pending_filter->handle == async_e3prom->active_filter_handle)) {
						// Remove this filter
						mal_pool_free(&async_e3prom->pending_filters_pool, current_pending_filter);
						continue;
					}
					// Save for next filter
					if (NULL == next_pending_filter) {
						next_pending_filter = current_pending_filter;
					}
				}
			}
			// Cleanup
			async_e3prom->switch_in_progress = false;
			async_e3prom->active_filter = NULL;
			async_e3prom->active_filter_complete = NULL;
			async_e3prom->active_filter_handle = NULL;
			result = mal_async_e3prom_handle_pending_writes(async_e3prom);
			// Start next filter
			if (NULL != next_pending_filter) {
				step_result = mal_async_e3prom_filter(async_e3prom, next_pending_filter->filter,
						                              next_pending_filter->filter_complete,
						                              next_pending_filter->handle);
				if (MAL_ERROR_OK != step_result) {
					result = step_result;
				}
			}
		} else if (MAL_ERROR_IN_PROGRESS == result) {
			// This is normal, this burst did not finish the transfer
			result = MAL_ERROR_OK;
		}
	}
	return result;
}

mal_error_e mal_async_e3prom_filter(mal_async_e3prom_s *async_e3prom, mal_e3prom_filter_t filter,
									mal_async_e3prom_filter_complete_t filter_complete, void *handle) {

	mal_error_e result;
	if (!async_e3prom->switch_in_progress) {
		// Make the active section decommissioned.
		result = mal_e3prom_start_active_sector_switch(&async_e3prom->e3prom, false);
		if (MAL_ERROR_OK != result) {
			return result;
		}
		async_e3prom->secsw_previous_key_address = mal_e3prom_get_previous_section_last_address(&async_e3prom->e3prom);
		result = mal_e3prom_switch_active_sector_step(&async_e3prom->e3prom, async_e3prom->burst_size,
													   &async_e3prom->secsw_previous_key_address, filter, handle);
		if (MAL_ERROR_IN_PROGRESS == result) {
			async_e3prom->switch_in_progress = true;
			async_e3prom->active_filter = filter;
			async_e3prom->active_filter_complete = filter_complete;
			async_e3prom->active_filter_handle = handle;
			result = MAL_ERROR_OK;
		} else if (MAL_ERROR_OK == result) {
			// Execute complete callback
			filter_complete(handle, async_e3prom);
		}
		return result;
	}
	// Save pending filter
	mal_async_e3prom_pending_filter_s *pending_filter;
	result = mal_pool_allocate(&async_e3prom->pending_filters_pool, (void**)&pending_filter);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	pending_filter->filter = filter;
	pending_filter->filter_complete = filter_complete;
	pending_filter->handle = handle;
	return MAL_ERROR_OK;
}

mal_error_e mal_async_e3prom_search(mal_async_e3prom_s *async_e3prom,
									mal_async_e3prom_search_delegate_t search_delegate,
									void *handle) {
	// Check pending writes
	uint64_t index;
	mal_async_e3prom_pending_write_s *pending_write;
	MAL_POOL_FOR_EACH(&async_e3prom->pending_writes_pool, index, pending_write) {
		mal_e3prom_search_result_e result = search_delegate(handle, async_e3prom, pending_write->key, 
				                                            pending_write->value);
		if (MAL_E3PROM_SEARCH_RESULT_FOUND == result) {
			return MAL_ERROR_OK;
		}
	}
	// Search active section
	uint64_t key_address;
	mal_e3prom_section_e active_section = async_e3prom->e3prom.active_section;
	uint64_t start_address = mal_flash_get_page_start_address(async_e3prom->e3prom.sections[active_section].start_page);
	uint64_t last_address = async_e3prom->e3prom.sections[active_section].last_address;
	uint32_t step = sizeof(uint32_t) * 2;
	for (key_address = last_address;
		 key_address >= start_address && key_address <= last_address;
		 key_address -= step) {
		uint32_t key = mal_flash_read_uint32((unsigned int)key_address);
		if (MAL_E3PROM_EMPTY_KEY == key) {
			continue;
		}
		uint64_t value_address = key_address + sizeof(uint32_t);
		uint32_t value = mal_flash_read_uint32((unsigned int)value_address);
		mal_e3prom_search_result_e result = search_delegate(handle, async_e3prom, key, value);
		if (MAL_E3PROM_SEARCH_RESULT_FOUND == result) {
			return MAL_ERROR_OK;
		}
	}
	return MAL_ERROR_NOT_FOUND;
}
