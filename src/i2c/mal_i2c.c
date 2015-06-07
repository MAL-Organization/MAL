/*
 * mal_i2c.c
 *
 *  Created on: Jun 1, 2015
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
 *
 * This file is part of MAL.
 *
 * MAL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "i2c/mal_i2c.h"
#include "hspec/mal_hspec.h"
#include "std/mal_error.h"

mal_error_e mal_i2c_init_master(mal_hspec_i2c_init_s *init) {
	mal_error_e result;
	// Check if interface is valid
	result = mal_hspec_is_i2c_interface_valid(init->interface, init->scl_gpio, init->sda_gpio);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialise as master interface
	return mal_hspec_i2c_master_init(init);
}
