/*
 * mal_circular_buffer.c
 *
 *  Created on: Jun 28, 2015
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

#include "mal_circular_buffer.h"

void mal_circular_buffer_init(void *data_buffer, uint64_t element_size, uint64_t size, mal_circular_buffer_s *buffer) {
	buffer->buffer = data_buffer;
	buffer->element_size = element_size;
	buffer->maximum_size = size / element_size;
	buffer->input_pointer = 0;
	buffer->output_pointer = 0;
	buffer->size = 0;
}

mal_error_e mal_circular_buffer_write(mal_circular_buffer_s *buffer, void *data) {
	// Check for space
	if (buffer->size >= buffer->maximum_size) {
		return MAL_ERROR_FULL;
	}
	// Write data
	uint64_t i;
	uint64_t offset = buffer->element_size * buffer->input_pointer;
	for (i = 0; i < buffer->element_size; i++) {
		buffer->buffer[offset + i] = ((uint8_t*)data)[i];
	}
	buffer->size++;
	// Increment input pointer and handle wrap around
	if (++buffer->input_pointer >= buffer->maximum_size) {
		buffer->input_pointer = 0;
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_circular_buffer_read(mal_circular_buffer_s *buffer, void *data) {
	// Check for space
	if (buffer->size <= 0) {
		return MAL_ERROR_EMPTY;
	}
	// Read data
	uint64_t i;
	uint64_t offset = buffer->element_size * buffer->output_pointer;
	for (i = 0; i < buffer->element_size; i++) {
		((uint8_t*)data)[i] = buffer->buffer[offset + i];
	}
	buffer->size--;
	// Increment input pointer and handle wrap around
	if (++buffer->output_pointer >= buffer->maximum_size) {
		buffer->output_pointer = 0;
	}

	return MAL_ERROR_OK;
}

void mal_circular_buffer_clear(mal_circular_buffer_s *buffer) {
	buffer->input_pointer = 0;
	buffer->output_pointer = 0;
	buffer->size = 0;
}

mal_error_e mal_circular_buffer_peek(mal_circular_buffer_s *buffer, uint64_t index, void *data) {
	// Check for space
	if (buffer->size <= 0) {
		return MAL_ERROR_EMPTY;
	}
	// Check index
	if (index >= buffer->maximum_size) {
		return MAL_ERROR_OUT_OF_BOUND;
	}
	// Check if index contains data
	if (index >= buffer->size) {
		return MAL_ERROR_OUT_OF_BOUND;
	}
	// Compute actual buffer index
	uint64_t buffer_index = buffer->output_pointer + index;
	buffer_index = buffer_index % buffer->maximum_size;
	// Read data
	uint64_t i;
	uint64_t offset = buffer->element_size * buffer_index;
	for (i = 0; i < buffer->element_size; i++) {
		((uint8_t*)data)[i] = buffer->buffer[offset + i];
	}

	return MAL_ERROR_OK;
}
