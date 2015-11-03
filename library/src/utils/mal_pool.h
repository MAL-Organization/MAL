/**
 * @file mal_pool.h
 * @author Olivier Allaire
 * @date October 4 2015
 * @copyright Copyright (c) 2015 Olivier Allaire
 * @par This file is part of MAL.
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
 * @brief Object pool utility. This will allow to emulate a memory heap while
 * having a deterministic memory usage.
 */

#ifndef UTILS_MAL_POOL_H_
#define UTILS_MAL_POOL_H_

#include "std/mal_bool.h"
#include "std/mal_stdint.h"
#include "std/mal_error.h"

/**
 * @defgroup Pool
 * @brief @copybrief mal_pool.h
 * @{
 */

/**
 * This structure is the base constituent of the pool. It is a basic container
 * to track an object pointer and its state.
 */
typedef struct {
	bool is_free; /**< A boolean value. It is true if it's free, false otherwise.*/
	void *object; /**< A pointer to the object.*/
} mal_pool_object_s;

/**
 * The pool object. This is a container for all the variables required to do
 * all pool operations.
 */
typedef struct {
	mal_pool_object_s *objects; /**< This is an array of ::mal_pool_object_s.*/
	uint64_t object_size; /**< This is the size of each object stored by #mal_pool_object_s in bytes*/
	uint64_t size; /**< The size of the pool. In other words, the size of the objects array.*/
} mal_pool_s;

/**
 * @brief This function will initialize an instance of a pool object.
 * @param object_pool This should be an array of #mal_pool_object_s.
 * @param pool_size The number of #mal_pool_object_s that fits in ::object_pool.
 * In other words, the size of the array.
 * @param object_buffers This is an array of objects casted to bytes. It could
 * be an array of a complex structure or simply an array of integers. pool_size
 * should also indicate how many object can fit into the buffers.
 * @param object_size The size, in bytes, of a single object.
 * @param pool The pool to initialize.
 * @return Returns #MAL_ERROR_OK upon completion.
 */
mal_error_e mal_pool_init(mal_pool_object_s *object_pool, uint64_t pool_size, uint8_t *object_buffers, uint64_t object_size, mal_pool_s *pool);

/**
 * @brief This function will attempt to allocate a free object.
 * @param pool The pool of objects to allocate from.
 * @param object If a free object is found, it will be stored in object.
 * @return If an object is found, #MAL_ERROR_OK will be returned.
 * #MAL_ERROR_EMPTY will be returned if no free object remains.
 */
mal_error_e mal_pool_allocate(mal_pool_s *pool, void **object);

/**
 * @brief This function will free an allocated object.
 * @param pool The pool to free the object from.
 * @param object The object to free.
 * @return Will always return #MAL_ERROR_OK, even if the object is free or not
 * found.
 */
mal_error_e mal_pool_free(mal_pool_s *pool, void *object);

/**
 * @}
 */

#endif /* UTILS_MAL_POOL_H_ */
