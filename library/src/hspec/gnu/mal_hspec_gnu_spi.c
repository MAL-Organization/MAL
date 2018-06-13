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

#include "mal_hspec_gnu_spi.h"
#include "std/mal_stdlib.h"
#include "std/mal_defs.h"

typedef struct {
	mal_spi_master_init_s init_parameters;
	mal_spi_msg_s *active_msg;
} gnu_spi_interface_s;

gnu_spi_interface_s interfaces[MAL_SPI_SIZE];

mal_error_e mal_spi_init_master(mal_spi_master_init_s *init, mal_spi_s *handle) {
	handle->interface = init->interface;
	// Save initialization parameters
	interfaces[init->interface].init_parameters = *init;
	// Initialize variables
	interfaces[init->interface].active_msg = NULL;

	return MAL_ERROR_OK;
}

mal_error_e mal_spi_start_transaction(mal_spi_s *handle, mal_spi_msg_s *msg) {
	if (NULL == interfaces[handle->interface].active_msg) {
		return MAL_ERROR_HARDWARE_UNAVAILABLE;
	}
	// Save message
	interfaces[handle->interface].active_msg = msg;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_gnu_spi_get_msg(mal_spi_e interface, mal_spi_msg_s **msg) {
	if (NULL == interfaces[interface].active_msg) {
		return MAL_ERROR_EMPTY;
	}
	*msg = interfaces[interface].active_msg;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_spi_end_transaction(mal_spi_e interface) {
	// Make sure message is not NULL
	if (NULL == interfaces[interface].active_msg) {
		return MAL_ERROR_EMPTY;
	}
	// Execute callback
	mal_spi_msg_s *next_msg = NULL;
	interfaces[interface].active_msg->callback(interfaces[interface].active_msg->handle,
                                               interfaces[interface].active_msg,
                                               &next_msg);
	interfaces[interface].active_msg = next_msg;

	return MAL_ERROR_OK;
}

MAL_DEFS_INLINE void mal_spi_disable_interrupt(mal_spi_s *handle, mal_spi_interrupt_state_s *state) {
	MAL_DEFS_UNUSED(handle);
	MAL_DEFS_UNUSED(state);
}

MAL_DEFS_INLINE void mal_spi_restore_interrupt(mal_spi_s *handle, mal_spi_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(state);
}
