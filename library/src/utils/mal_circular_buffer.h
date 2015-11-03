/**
 * @file mal_circular_buffer.h
 * @author Olivier Allaire
 * @date June 28 2015
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
 * @brief Circular buffer utility. This allow you to have a circular buffer of
 * a user defined type.
 */

#ifndef UTILS_MAL_CIRCULAR_BUFFER_H_
#define UTILS_MAL_CIRCULAR_BUFFER_H_

#include "std/mal_stdint.h"
#include "std/mal_error.h"

/**
 * @defgroup CircularBuffer
 * @brief @copybrief mal_circular_buffer.h
 * @{
 */

/**
 * A structure containing all the variables of circular buffer.
 */
typedef struct {
	uint8_t *buffer; /**< A byte buffer that will contain individual objects.*/
	uint64_t element_size; /**< The size of one object in bytes.*/
	uint64_t input_pointer; /**< Points to the current object to write.*/
	uint64_t output_pointer; /**< Points to the current object to read.*/
	uint64_t size; /**< The size in number of objects.*/
	uint64_t maximum_size; /**< The buffer size in objects.*/
} mal_circular_buffer_s;

/**
 * @brief Initialize a circular buffer.
 * @param data_buffer a buffer of bytes that should multiples of the object you
 * want to use.
 * @param element_size The size of one object.
 * @param size  The size of the data buffer in bytes.
 * @param buffer The buffer to initialize.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_circular_buffer_init(void *data_buffer, uint64_t element_size, uint64_t size, mal_circular_buffer_s *buffer);

/**
 * @brief Write to a circular buffer.
 * @param buffer The buffer to write to.
 * @param data The object to write into the buffer.
 * @return Returns #MAL_ERROR_OK on success. If the buffer is full, it will
 * return #MAL_ERROR_FULL.
 */
mal_error_e mal_circular_buffer_write(mal_circular_buffer_s *buffer, void *data);

/**
 * @brief Read from a circular buffer.
 * @param buffer The buffer to read from.
 * @param data The read object will be returned in data.
 * @return Returns #MAL_ERROR_OK on success. If the buffer is empty, it will
 * return #MAL_ERROR_EMPTY.
 */
mal_error_e mal_circular_buffer_read(mal_circular_buffer_s *buffer, void *data);

/**
 * @brief Clears all data in buffer.
 * @param buffer The buffer to clear.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_circular_buffer_clear(mal_circular_buffer_s *buffer);

/**
 * @}
 */

#endif /* UTILS_MAL_CIRCULAR_BUFFER_H_ */
