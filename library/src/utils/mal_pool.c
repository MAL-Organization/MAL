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

mal_error_e mal_pool_init(mal_pool_object_s *object_pool, uint64_t pool_size, uint8_t *object_buffers, uint64_t object_size, mal_pool_s *pool) {
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

	return MAL_ERROR_OK;
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

mal_error_e mal_pool_free(mal_pool_s *pool, void *object) {
	uint64_t index;
	for (index = 0; index < pool->size; index++) {
		if (pool->objects[index].object == object) {
			pool->objects[index].is_free = true;
			return MAL_ERROR_OK;
		}
	}
	return MAL_ERROR_OK;
}
