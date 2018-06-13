/*
 * Copyright (c) 2018 Olivier Allaire
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

#include "mal_hspec_gnu_can.h"
#include "utils/mal_circular_buffer.h"
#include "std/mal_defs.h"

#define MESSAGE_BUFFER_SIZE	3

typedef struct {
	mal_can_init_s init;
	mal_can_s *handle;
	mal_circular_buffer_s tx_circular_buffer;
	mal_can_msg_s message_buffer[MESSAGE_BUFFER_SIZE];
} gnu_can_interface_s;

static gnu_can_interface_s can_interfaces[MAL_CAN_SIZE];

mal_error_e mal_can_init(mal_can_init_s *init, mal_can_s *handle) {
	handle->interface = init->interface;
	// Save init
	can_interfaces[init->interface].init = *init;
	can_interfaces[init->interface].handle = handle;
	// Initialise circular buffer
	mal_circular_buffer_init((void*)can_interfaces[init->interface].message_buffer,
							 sizeof(mal_can_msg_s),
							 sizeof(mal_can_msg_s) * MESSAGE_BUFFER_SIZE,
							 &can_interfaces[init->interface].tx_circular_buffer);
	return MAL_ERROR_OK;
}

mal_error_e mal_can_direct_init(mal_can_init_s *init, const void *direct_init, mal_can_s *handle) {
	MAL_DEFS_UNUSED(direct_init);
    return mal_can_init(init, handle);
}

mal_error_e mal_can_transmit(mal_can_s *handle, mal_can_msg_s *msg) {
	mal_error_e result;
	// Write to buffer
	result = mal_circular_buffer_write(&can_interfaces[handle->interface].tx_circular_buffer, msg);
	if (MAL_ERROR_OK != result) {
		return MAL_ERROR_HARDWARE_UNAVAILABLE;
	}
	return result;
}

mal_error_e mal_hspec_gnu_can_get_tx_msg(mal_can_e interface, mal_can_msg_s *msg) {
	mal_error_e result;
	// Remove message from buffer
	result = mal_circular_buffer_read(&can_interfaces[interface].tx_circular_buffer, msg);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Execute tx callback
	mal_can_msg_s next_msg;
	result = can_interfaces[interface].init.tx_callback(can_interfaces[interface].init.tx_callback_handle, &next_msg);
	if (MAL_ERROR_OK == result) {
	    mal_can_transmit(can_interfaces[interface].handle, &next_msg);
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_can_add_filter(mal_can_s *handle, mal_can_filter_s *filter) {
	MAL_DEFS_UNUSED(handle);
	MAL_DEFS_UNUSED(filter);
	return MAL_ERROR_OK;
}

mal_error_e mal_can_remove_filter(mal_can_s *handle, mal_can_filter_s *filter) {
	MAL_DEFS_UNUSED(handle);
	MAL_DEFS_UNUSED(filter);
	return MAL_ERROR_OK;
}

void mal_hspec_gnu_can_push_rx_msg(mal_can_e interface, mal_can_msg_s *msg) {
	return can_interfaces[interface].init.rx_callback(can_interfaces[interface].init.rx_callback_handle, msg);
}

mal_error_e mal_can_deinit(mal_can_s *handle) {
    MAL_DEFS_UNUSED(handle);
}

MAL_DEFS_INLINE void mal_can_disable_interrupt(mal_can_s *handle, mal_can_interrupt_state_s *state) {
	MAL_DEFS_UNUSED(handle);
	MAL_DEFS_UNUSED(state);
}

MAL_DEFS_INLINE void mal_can_restore_interrupt(mal_can_s *handle, mal_can_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(state);
}
