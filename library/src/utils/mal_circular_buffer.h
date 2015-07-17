/*
 * mal_circular_buffer.h
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
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_MAL_CIRCULAR_BUFFER_H_
#define UTILS_MAL_CIRCULAR_BUFFER_H_

#include "std/mal_stdint.h"
#include "std/mal_error.h"

typedef struct {
	uint8_t *buffer;
	uint64_t element_size;
	uint64_t input_pointer;
	uint64_t output_pointer;
	uint64_t size;
	uint64_t maximum_size;
} mal_circular_buffer_s;

mal_error_e mal_circular_buffer_init(void *data_buffer, uint64_t element_size, uint64_t size, mal_circular_buffer_s *buffer);

mal_error_e mal_circular_buffer_write(mal_circular_buffer_s *buffer, void *data);

mal_error_e mal_circular_buffer_read(mal_circular_buffer_s *buffer, void *data);

mal_error_e mal_circular_buffer_clear(mal_circular_buffer_s *buffer);

#endif /* UTILS_MAL_CIRCULAR_BUFFER_H_ */
