/*
 * mal_i2c_buffer.c
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

#include "std/mal_stdlib.h"
#include "mal_i2c_buffer.h"
#include "std/mal_bool.h"

static void i2c_callback(void *handle, mal_i2c_packet_s *packet, mal_i2c_result_e result, mal_i2c_msg_s **next_msg);

static void
mal_i2c_buffer_common_init(mal_i2c_buffer_init_s *init, mal_i2c_buffer_handle_s *buffer_handle, mal_i2c_s *i2c_handle) {
	// Initialise buffer
	mal_circular_buffer_init(init->buffer,
							 sizeof(mal_i2c_msg_s),
							 init->buffer_size,
							 (mal_circular_buffer_s*)&buffer_handle->buffer);
	// Save interface
	buffer_handle->handle = i2c_handle;
}

mal_error_e
mal_i2c_buffer_init(mal_i2c_buffer_init_s *init, mal_i2c_buffer_handle_s *buffer_handle, mal_i2c_s *i2c_handle) {
	mal_error_e result;
	// Initialise I2C interface
	result = mal_i2c_init_master(&init->i2c_init, i2c_handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Common init
	mal_i2c_buffer_common_init(init, buffer_handle, i2c_handle);

	return MAL_ERROR_OK;
}

mal_error_e mal_i2c_buffer_direct_init(mal_i2c_buffer_init_s *init, const void *direct_init,
                                       mal_i2c_buffer_handle_s *buffer_handle, mal_i2c_s *i2c_handle) {
	mal_error_e result;
	// Initialise I2C interface
	result = mal_i2c_master_direct_init(&init->i2c_init, direct_init, i2c_handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Common init
	mal_i2c_buffer_common_init(init, buffer_handle, i2c_handle);

	return MAL_ERROR_OK;
}

mal_error_e mal_i2c_buffer_write(mal_i2c_buffer_handle_s *handle, mal_i2c_msg_s *msg) {
	mal_error_e result = MAL_ERROR_HARDWARE_UNAVAILABLE;
	mal_i2c_interrupt_state_s state;
	mal_i2c_disable_interrupt(handle->handle, &state);
	// Try to send message
	if (NULL == handle->callback) {
		// Copy msg to active msg
		handle->active_msg.callback = msg->callback;
		handle->active_msg.handle = msg->handle;
        handle->active_msg.packet = msg->packet;
		// Swap callbacks and handle
        handle->callback = handle->active_msg.callback;
		handle->active_msg.callback = &i2c_callback;
		handle->callback_handle = handle->active_msg.handle;
        handle->active_msg.handle = handle;
		// Start transfer
		result = mal_i2c_transfer(handle->handle,
								  (mal_i2c_msg_s*)&handle->active_msg);
		if (MAL_ERROR_OK != result) {
			// Failed to write message restore callback for queue
			handle->active_msg.callback = handle->callback;
			handle->callback = NULL;
		}
	}
	if (MAL_ERROR_HARDWARE_UNAVAILABLE == result) {
		result = mal_circular_buffer_write((mal_circular_buffer_s*)&handle->buffer, msg);
	}
	mal_i2c_restore_interrupt(handle->handle, &state);
	return result;
}

static void i2c_callback(void *handle, mal_i2c_packet_s *packet, mal_i2c_result_e result, mal_i2c_msg_s **next_msg) {
	mal_i2c_buffer_handle_s *buffer_handle = handle;
	buffer_handle->cb_next_msg = NULL;
	// Execute callback
	if (NULL != buffer_handle->callback) {
        buffer_handle->callback((void*)buffer_handle->callback_handle, packet, result, &buffer_handle->cb_next_msg);
	}
	// Send next message. The first source is the callback of the message. If
	// not, use the buffer.
	*next_msg = buffer_handle->cb_next_msg;
	if (NULL == buffer_handle->cb_next_msg) {
		mal_error_e mal_result;
		mal_result = mal_circular_buffer_read((mal_circular_buffer_s*)&buffer_handle->buffer,
				                              (void*)&buffer_handle->active_msg);
		if (MAL_ERROR_OK == mal_result) {
			*next_msg = (mal_i2c_msg_s*)&buffer_handle->active_msg;
		} else {
			*next_msg = NULL;
		}
	}
	// Replace callback
	if (NULL != *next_msg) {
        buffer_handle->callback = (*next_msg)->callback;
		(*next_msg)->callback = &i2c_callback;
        buffer_handle->callback_handle = (*next_msg)->handle;
        (*next_msg)->handle = buffer_handle;
	} else {
        buffer_handle->callback = NULL;
	}
}
