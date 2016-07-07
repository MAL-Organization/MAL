/*
 * mal_hspec_mingw_spi.c
 *
 *  Created on: May 19, 2016
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

#include "mal_hspec_mingw_spi.h"
#include "mal_hspec_mingw_cmn.h"
#include "std/mal_stdlib.h"

typedef struct {
	mal_hspec_spi_master_init_s init_parameters;
	mal_hspec_spi_msg_s *active_msg;
} mingw_spi_interface_s;

mingw_spi_interface_s interfaces[MAL_HSPEC_SPI_SIZE];

mal_error_e mal_hspec_mingw_get_valid_spi_ios(mal_hspec_spi_e interface,
											  const mal_hspec_gpio_s **mosis,
											  uint8_t *mosis_size,
											  const mal_hspec_gpio_s **misos,
											  uint8_t *misos_size,
											  const mal_hspec_gpio_s **clks,
											  uint8_t *clks_size,
											  const mal_hspec_gpio_s **selects,
											  uint8_t *selects_size) {
	// Fetch IOs
	mal_hspec_mingw_cmn_valid_ios(mosis, mosis_size);
	*misos = *mosis;
	*misos_size = *mosis_size;
	*clks = *mosis;
	*clks_size = *mosis_size;
	*selects = *mosis;
	*selects_size = *mosis_size;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_spi_master_init(mal_hspec_spi_master_init_s *init) {
	// Save initialization parameters
	interfaces[init->interface].init_parameters = *init;
	// Initialize variables
	interfaces[init->interface].active_msg = NULL;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_spi_start_transaction(mal_hspec_spi_e interface, mal_hspec_spi_msg_s *msg) {
	if (NULL == interfaces[interface].active_msg) {
		return MAL_ERROR_HARDWARE_UNAVAILABLE;
	}
	// Save message
	interfaces[interface].active_msg = msg;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_spi_get_msg(mal_hspec_spi_e interface, mal_hspec_spi_msg_s **msg) {
	if (NULL == interfaces[interface].active_msg) {
		return MAL_ERROR_EMPTY;
	}
	*msg = interfaces[interface].active_msg;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_spi_end_transaction(mal_hspec_spi_e interface) {
	// Make sure message is not NULL
	if (NULL == interfaces[interface].active_msg) {
		return MAL_ERROR_EMPTY;
	}
	// Execute callback
	mal_hspec_spi_msg_s *next_msg = NULL;
	interfaces[interface].active_msg->callback(interfaces[interface].active_msg, &next_msg);
	interfaces[interface].active_msg = next_msg;

	return MAL_ERROR_OK;
}
