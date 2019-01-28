/*
 * mal_pool.c
 *
 *  Created on: Oct 4, 2015
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

#include "mal_pool.h"

void mal_pool_init(mal_pool_object_s *object_pool, uint64_t pool_size, uint8_t *object_buffers, uint64_t object_size, mal_pool_s *pool) {
	// Initialise pool
	pool->object_size = object_size;
	pool->objects = object_pool;
	pool->size = pool_size;
	// Initialise objects
	uint64_t object;
	uint64_t object_offset = 0;
	for (object = 0; object < pool->size; object++) {
		pool->objects[object].is_free = true;
		pool->objects[object].object = object_buffers + object_offset;
		object_offset += object_size;
	}
}

mal_error_e mal_pool_allocate(mal_pool_s *pool, void **object) {
	uint64_t index;
	for (index = 0; index < pool->size; index++) {
		if (pool->objects[index].is_free) {
			pool->objects[index].is_free = false;
			*object = pool->objects[index].object;
			return MAL_ERROR_OK;
		}
	}
	return MAL_ERROR_EMPTY;
}

mal_error_e mal_pool_allocate_array(mal_pool_s *pool, uint64_t array_size, void **array_start) {
	// Validate array size
	if (array_size <= 0) {
		return MAL_ERROR_OPERATION_INVALID;
	} else if (1 == array_size) {
		return mal_pool_allocate(pool, array_start);
	}
	// Find array
	uint64_t start_index;
	for (start_index = 0; start_index < pool->size; start_index++) {
		if (!pool->objects[start_index].is_free) {
			continue;
		}
		// Start index found, check if there is enough space ahead.
		uint64_t index;
		bool found = true;
		for (index = 1; index < array_size; index++) {
			// Validate current index
			uint64_t current_index = start_index + index;
			if (current_index >= pool->size) {
				return MAL_ERROR_NOT_FOUND;
			}
			// Check if index is free
			if (pool->objects[current_index].is_free) {
				continue;
			}
			// Array is not long enough, move start index forward to prevent check this space too many times.
			start_index = current_index;
			found = false;
			break;
		}
		if (!found) {
			continue;
		}
		// Array found, allocate
		*array_start = pool->objects[start_index].object;
		for (index = 0; index < array_size; index++) {
			uint64_t current_index = start_index + index;
			pool->objects[current_index].is_free = false;
		}
		return MAL_ERROR_OK;
	}
	return MAL_ERROR_NOT_FOUND;
}

void mal_pool_free(mal_pool_s *pool, void *object) {
	uint64_t index;
	for (index = 0; index < pool->size; index++) {
		if (pool->objects[index].object == object) {
			pool->objects[index].is_free = true;
		}
	}
}

void mal_pool_free_array(mal_pool_s *pool, void *array_start, uint64_t array_size) {
	uint64_t start_index;
	for (start_index = 0; start_index < pool->size; start_index++) {
		if (pool->objects[start_index].object != array_start) {
			continue;
		}
		// Free all array
		uint64_t index;
		for (index = 0; index < array_size; index++) {
			uint64_t current_index = start_index + index;
			if (current_index >= pool->size) {
				return;
			}
			pool->objects[current_index].is_free = true;
		}
	}
}

void mal_pool_flush(mal_pool_s *pool) {
	uint64_t index;
	for (index = 0; index < pool->size; index++) {
		pool->objects[index].is_free = true;
	}
}

mal_error_e mal_pool_get_next(mal_pool_s *pool, uint64_t *index, void **object) {
    for (; *index < pool->size; (*index)++) {
        if (!pool->objects[*index].is_free) {
            // Index is incremented so the next call will start at the next index
            *object = pool->objects[(*index)++].object;
            return MAL_ERROR_OK;
        }
    }
    return MAL_ERROR_NOT_FOUND;
}
