/*
 * mal_can_buffer.c
 *
 *  Created on: Jul 5, 2015
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

#include "mal_can_buffer.h"
#include "can/mal_can.h"
#include "std/mal_stdlib.h"
#include "std/mal_bool.h"

static mal_error_e tx_callback(void *handle, mal_can_msg_s *next_msg);
static void rx_callback(void *handle, mal_can_msg_s *msg);
static void mal_can_buffer_init_common(mal_can_buffer_init_s *init, mal_can_buffer_handle_s *buffer_handle,
									   mal_can_s *can_handle);

static void mal_can_buffer_init_common(mal_can_buffer_init_s *init, mal_can_buffer_handle_s *buffer_handle,
                                       mal_can_s *can_handle) {
	// Initialise tx circular buffer
	mal_circular_buffer_init(init->tx_buffer, sizeof(mal_can_msg_s), init->tx_buffer_size, (mal_circular_buffer_s*)&buffer_handle->tx_buffer);
	// Initialise rx circular buffer
	mal_circular_buffer_init(init->rx_buffer, sizeof(mal_can_msg_s), init->rx_buffer_size, (mal_circular_buffer_s*)&buffer_handle->rx_buffer);
	// Save interface
	buffer_handle->handle = can_handle;
}

mal_error_e mal_can_buffer_direct_init(mal_can_buffer_init_s *init, const void *direct_init,
                                       mal_can_buffer_handle_s *buffer_handle, mal_can_s *can_handle) {
	mal_error_e result;
	// Initialise CAN interface
	mal_can_init_s can_init;
	can_init.interface = init->interface;
	can_init.tx_port = init->tx_port;
    can_init.tx_pin = init->tx_pin;
    can_init.rx_port = init->rx_port;
    can_init.rx_pin = init->rx_pin;
	can_init.bitrate = init->bitrate;
	can_init.tx_callback = &tx_callback;
	can_init.tx_callback_handle = buffer_handle;
	can_init.rx_callback = &rx_callback;
	can_init.rx_callback_handle = buffer_handle;
	result = mal_can_direct_init(&can_init, direct_init, can_handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialize common part
	mal_can_buffer_init_common(init, buffer_handle, can_handle);

	return MAL_ERROR_OK;
}

mal_error_e mal_can_buffer_init(mal_can_buffer_init_s *init, mal_can_buffer_handle_s *buffer_handle,
                                mal_can_s *can_handle) {
	mal_error_e result;
	// Initialise CAN interface
	mal_can_init_s can_init;
	can_init.interface = init->interface;
    can_init.tx_port = init->tx_port;
    can_init.tx_pin = init->tx_pin;
    can_init.rx_port = init->rx_port;
    can_init.rx_pin = init->rx_pin;
	can_init.bitrate = init->bitrate;
    can_init.tx_callback = &tx_callback;
    can_init.tx_callback_handle = buffer_handle;
    can_init.rx_callback = &rx_callback;
    can_init.rx_callback_handle = buffer_handle;
	result = mal_can_init(&can_init, can_handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialize common part
	mal_can_buffer_init_common(init, buffer_handle, can_handle);

	return MAL_ERROR_OK;
}

mal_error_e mal_can_buffer_write(mal_can_buffer_handle_s *handle, mal_can_msg_s *msg) {
	mal_error_e result;
	mal_can_interrupt_state_s state;
	mal_can_disable_interrupt(handle->handle, &state);
	result = mal_can_transmit(handle->handle, msg);
	if (MAL_ERROR_HARDWARE_UNAVAILABLE == result) {
		// Interface is busy writing, write to buffer
		result = mal_circular_buffer_write((mal_circular_buffer_s*)&handle->tx_buffer, msg);
	}
	mal_can_restore_interrupt(handle->handle, &state);

	return result;
}

mal_error_e mal_can_buffer_read(mal_can_buffer_handle_s *handle, mal_can_msg_s *msg) {
	mal_error_e result;
    mal_can_interrupt_state_s state;
	mal_can_disable_interrupt(handle->handle, &state);
	result = mal_circular_buffer_read((mal_circular_buffer_s*)&handle->rx_buffer, msg);
	mal_can_restore_interrupt(handle->handle, &state);

	return result;
}

static mal_error_e tx_callback(void *handle, mal_can_msg_s *next_msg) {
	mal_error_e result;
	mal_can_buffer_handle_s *can_buffer = handle;
	result = mal_circular_buffer_read((mal_circular_buffer_s*)&can_buffer->tx_buffer, next_msg);
	if (MAL_ERROR_OK != result) {
		return MAL_ERROR_EMPTY;
	}
	return MAL_ERROR_OK;
}

static void rx_callback(void *handle, mal_can_msg_s *msg) {
    mal_can_buffer_handle_s *can_buffer = handle;
	mal_circular_buffer_write((mal_circular_buffer_s*)&can_buffer->rx_buffer, msg);
}
