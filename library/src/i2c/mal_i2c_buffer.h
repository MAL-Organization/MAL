/**
 * @file mal_i2c_buffer.h
 * @author Olivier Allaire
 * @date October 3 2015
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
 * @brief Functions to queue I2C messages for an I2C interface. This is useful
 * when multiple modules will use the same I2C interface.
 */

#ifndef I2C_MAL_I2C_BUFFER_H_
#define I2C_MAL_I2C_BUFFER_H_

#include "mal_i2c.h"
#include "std/mal_stdint.h"
#include "std/mal_error.h"
#include "utils/mal_circular_buffer.h"

/**
 * @addtogroup I2C
 * @{
 */

/**
 * The initialisation parameters of an I2C buffer.
 */
typedef struct {
	mal_hspec_i2c_init_s i2c_init; /**< The I2C interface initialisation parameters.*/
	uint8_t *buffer; /**< A pointer to an array of bytes. Should be a multiple of ::mal_hspec_i2c_msg_s.*/
	uint64_t buffer_size; /**< The size of the buffer in bytes.*/
} mal_i2c_buffer_init_s;

/**
 * A structure that contains all the necessary variables for the I2C buffer.
 */
typedef struct {
	mal_hspec_i2c_e interface; /**< The I2C interface.*/
	mal_circular_buffer_s buffer; /**< A circular buffer.*/
} mal_i2c_buffer_handle_s;

/**
 * @brief Function to initialise an I2C buffer.
 * @param init The initialisation parameters.
 * @param handle A handle to initialise.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_i2c_buffer_init(mal_i2c_buffer_init_s *init, mal_i2c_buffer_handle_s *handle);

/**
 * @brief Write a message to the buffer.
 * @param handle The handle to write to.
 * @param msg The message to write.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_i2c_buffer_write(mal_i2c_buffer_handle_s *handle, mal_hspec_i2c_msg_s *msg);

/**
 * @}
 */

#endif /* I2C_MAL_I2C_BUFFER_H_ */
