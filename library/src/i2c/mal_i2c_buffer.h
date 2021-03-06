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
 * The initialization parameters of an I2C buffer.
 */
typedef struct {
	mal_i2c_init_s i2c_init; /**< The I2C interface initialization parameters.*/
	uint8_t *buffer; /**< A pointer to an array of bytes. Should be a multiple of ::mal_hspec_i2c_msg_s.*/
	uint64_t buffer_size; /**< The size of the buffer in bytes.*/
} mal_i2c_buffer_init_s;

/**
 * A structure that contains all the necessary variables for the I2C buffer.
 */
typedef struct {
	mal_i2c_s *handle; /**< The I2C interface.*/
	volatile mal_circular_buffer_s buffer; /**< A circular buffer.*/
	volatile mal_i2c_msg_s active_msg; /**< The active message of the buffer.*/
    volatile mal_i2c_callback_t callback;
    volatile void *callback_handle;
    mal_i2c_msg_s *cb_next_msg;
} mal_i2c_buffer_handle_s;

/**
 * @brief Function to initialize an I2C buffer.
 * @param init The initialization parameters.
 * @param buffer_handle A handle to initialize. Used to access subsequent i2c buffer calls.
 * @param i2c_handle The I2C handle to initialize. This handle is used to access
 * subsequent I2C functions.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_i2c_buffer_init(mal_i2c_buffer_init_s *init, mal_i2c_buffer_handle_s *buffer_handle,
                                mal_i2c_s *i2c_handle);

/**
 * @brief Initialize an I2C buffer and an I2C interface using direct initialization.
 * @param init The initialization parameters.
 * @param direct_init A pointer to direct initialization parameter. See the
 * hardware specific implementation to know what type this should be.
 * @param buffer_handle A handle to initialize. Used to access subsequent i2c buffer calls.
 * @param i2c_handle The I2C handle to initialize. This handle is used to access
 * subsequent I2C functions.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_i2c_buffer_direct_init(mal_i2c_buffer_init_s *init, const void *direct_init,
                                       mal_i2c_buffer_handle_s *buffer_handle, mal_i2c_s *i2c_handle);

/**
 * @brief Write a message to the buffer.
 * @param handle The handle to write to.
 * @param msg The message to write.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_i2c_buffer_write(mal_i2c_buffer_handle_s *handle, mal_i2c_msg_s *msg);

/**
 * @}
 */

#endif /* I2C_MAL_I2C_BUFFER_H_ */
