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

static mal_error_e tx_callback(mal_hspec_can_e interface, mal_hspec_can_msg_s *next_msg);
static mal_error_e rx_callback(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg);

static mal_can_buffer_handle_s *handles[MAL_HSPEC_CAN_SIZE];

mal_error_e mal_can_buffer_init(mal_can_buffer_handle_s *handle, mal_can_buffer_init_s *init) {
	mal_error_e result;
	// Initialise CAN interface
	mal_hspec_can_init_s can_init;
	can_init.interface = init->interface;
	can_init.tx_gpio = init->tx_gpio;
	can_init.rx_gpio = init->rx_gpio;
	can_init.bitrate = init->bitrate;
	can_init.tx_callback = &tx_callback;
	can_init.rx_callback = &rx_callback;
	result = mal_can_init(&can_init);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialise tx circular buffer
	result = mal_circular_buffer_init(init->tx_buffer, sizeof(mal_hspec_can_msg_s), init->tx_buffer_size, (mal_circular_buffer_s*)&handle->tx_buffer);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialise rx circular buffer
	result = mal_circular_buffer_init(init->rx_buffer, sizeof(mal_hspec_can_msg_s), init->rx_buffer_size, (mal_circular_buffer_s*)&handle->rx_buffer);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Save interface
	handle->interface = init->interface;
	handles[handle->interface] = handle;

	return MAL_ERROR_OK;
}

mal_error_e mal_can_buffer_write(mal_can_buffer_handle_s *handle, mal_hspec_can_msg_s *msg) {
	mal_error_e result;
	bool active = mal_can_disable_interrupt(handle->interface);
	result = mal_can_transmit(handle->interface, msg);
	if (MAL_ERROR_HARDWARE_UNAVAILABLE == result) {
		// Interface is busy writing, write to buffer
		result = mal_circular_buffer_write((mal_circular_buffer_s*)&handle->tx_buffer, msg);
	}
	mal_can_enable_interrupt(handle->interface, active);

	return result;
}

mal_error_e mal_can_buffer_read(mal_can_buffer_handle_s *handle, mal_hspec_can_msg_s *msg) {
	mal_error_e result;
	bool active = mal_can_disable_interrupt(handle->interface);
	result = mal_circular_buffer_read((mal_circular_buffer_s*)&handle->rx_buffer, msg);
	mal_can_enable_interrupt(handle->interface, active);

	return result;
}

static mal_error_e tx_callback(mal_hspec_can_e interface, mal_hspec_can_msg_s *next_msg) {
	mal_error_e result;
	result = mal_circular_buffer_read((mal_circular_buffer_s*)&handles[interface]->tx_buffer, next_msg);
	if (MAL_ERROR_OK != result) {
		return MAL_ERROR_EMPTY;
	}
	return MAL_ERROR_OK;
}

static mal_error_e rx_callback(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg) {
	mal_circular_buffer_write((mal_circular_buffer_s*)&handles[interface]->rx_buffer, msg);
	return MAL_ERROR_OK;
}
