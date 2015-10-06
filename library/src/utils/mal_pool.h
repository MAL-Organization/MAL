/*
 * mal_pool.h
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

#ifndef UTILS_MAL_POOL_H_
#define UTILS_MAL_POOL_H_

#include "std/mal_bool.h"
#include "std/mal_stdint.h"
#include "std/mal_error.h"

typedef struct {
	bool is_free;
	void *object;
} mal_pool_object_s;

typedef struct {
	mal_pool_object_s *objects;
	uint64_t object_size;
	uint64_t size;
} mal_pool_s;

mal_error_e mal_pool_init(mal_pool_object_s *object_pool, uint64_t pool_size, uint8_t *object_buffers, uint64_t object_size, mal_pool_s *pool);

mal_error_e mal_pool_allocate(mal_pool_s *pool, void **object);

mal_error_e mal_pool_free(mal_pool_s *pool, void *object);

#endif /* UTILS_MAL_POOL_H_ */
