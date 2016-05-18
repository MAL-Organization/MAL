/*
 * mal_spi.c
 *
 *  Created on: May 18, 2016
 *      Author: olivi
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

#include "mal_spi.h"

mal_error_e mal_spi_init_master(mal_hspec_spi_init_s *init) {
	mal_error_e result;
	// Check if interface is valid
	result = mal_hspec_is_spi_interface_valid(init->interface, init->mosi, init->miso, init->clk, init->select);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialize as master interface
	return mal_hspec_spi_master_init(init);
}
