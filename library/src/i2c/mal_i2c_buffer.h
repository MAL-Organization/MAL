/*
 * mal_i2c_buffer.h
 *
 *  Created on: Oct 3, 2015
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

#ifndef I2C_MAL_I2C_BUFFER_H_
#define I2C_MAL_I2C_BUFFER_H_

#include "mal_i2c.h"
#include "std/mal_stdint.h"
#include "std/mal_error.h"
#include "utils/mal_circular_buffer.h"

typedef struct {
	mal_hspec_i2c_init_s i2c_init;
	uint8_t *buffer;
	uint64_t buffer_size;
} mal_i2c_buffer_init_s;

typedef struct {
	mal_hspec_i2c_e interface;
	mal_circular_buffer_s buffer;
} mal_i2c_buffer_handle_s;

mal_error_e mal_i2c_buffer_init(mal_i2c_buffer_init_s *init, mal_i2c_buffer_handle_s *handle);

mal_error_e mal_i2c_buffer_write(mal_i2c_buffer_handle_s *handle, mal_hspec_i2c_msg_s *msg);

#endif /* I2C_MAL_I2C_BUFFER_H_ */
