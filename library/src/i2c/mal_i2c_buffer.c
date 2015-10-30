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

typedef struct {
	volatile mal_hspec_i2c_callback_t callback;
	mal_i2c_buffer_handle_s *buffer_handle;
} i2c_interface_handle_s;

static mal_error_e i2c_callback(mal_hspec_i2c_e interface, mal_hspec_i2c_packet_s *packet, mal_hspec_i2c_result_e result, mal_hspec_i2c_msg_s **next_msg);

static i2c_interface_handle_s i2c_interface_handles[MAL_HSPEC_I2C_SIZE];

mal_error_e mal_i2c_buffer_init(mal_i2c_buffer_init_s *init, mal_i2c_buffer_handle_s *handle) {
	mal_error_e result;
	// Initialise I2C interface
	result = mal_i2c_init_master(&init->i2c_init);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialise buffer
	result = mal_circular_buffer_init(init->buffer, sizeof(mal_hspec_i2c_msg_s), init->buffer_size, &handle->buffer);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Save interface
	handle->interface = init->i2c_init.interface;
	i2c_interface_handles[handle->interface].buffer_handle = handle;

	return MAL_ERROR_OK;
}

mal_error_e mal_i2c_buffer_write(mal_i2c_buffer_handle_s *handle, mal_hspec_i2c_msg_s *msg) {
	mal_error_e result = MAL_ERROR_HARDWARE_UNAVAILABLE;
	bool active = mal_i2c_disable_interrupt(handle->interface);
	// Try to send message
	if (NULL == i2c_interface_handles[handle->interface].callback) {
		i2c_interface_handles[handle->interface].callback = msg->callback;
		msg->callback = &i2c_callback;
		result = mal_i2c_transfer(handle->interface, msg);
		if (MAL_ERROR_OK != result) {
			// Failed to write message restore callback for queue
			msg->callback = i2c_interface_handles[handle->interface].callback;
		}
	}
	if (MAL_ERROR_HARDWARE_UNAVAILABLE == result) {
		result = mal_circular_buffer_write(&handle->buffer, msg);
	}
	mal_i2c_enable_interrupt(handle->interface, active);
	return result;
}

static mal_error_e i2c_callback(mal_hspec_i2c_e interface, mal_hspec_i2c_packet_s *packet, mal_hspec_i2c_result_e result, mal_hspec_i2c_msg_s **next_msg) {
	static mal_hspec_i2c_msg_s buffer_msg;
	static mal_hspec_i2c_msg_s *cb_next_msg;
	cb_next_msg = NULL;
	// Execute callback
	if (NULL != i2c_interface_handles[interface].callback) {
		i2c_interface_handles[interface].callback(interface, packet, result, &cb_next_msg);
	}
	// Send next message. The first source is the callback of the message. If
	// not, use the buffer.
	next_msg = &cb_next_msg;
	if (NULL == cb_next_msg) {
		mal_error_e result;
		result = mal_circular_buffer_read(&i2c_interface_handles[interface].buffer_handle->buffer, &buffer_msg);
		if (MAL_ERROR_OK == result) {
			cb_next_msg = &buffer_msg;
		} else {
			cb_next_msg = NULL;
		}
	}
	// Replace callback
	if (NULL != cb_next_msg) {
		i2c_interface_handles[interface].callback = cb_next_msg->callback;
		cb_next_msg->callback = &i2c_callback;
	}
	return MAL_ERROR_OK;
}
