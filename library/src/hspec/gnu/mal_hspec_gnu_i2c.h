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

#ifndef HSPEC_GNU_MAL_HSPEC_GNU_I2C_H_
#define HSPEC_GNU_MAL_HSPEC_GNU_I2C_H_

#include "std/mal_bool.h"
#include "i2c/mal_i2c.h"

mal_error_e mal_hspec_gnu_i2c_get_transfer_msg(mal_i2c_e interface, mal_i2c_msg_s *msg);

bool mal_hspec_gnu_i2c_lock_interface(mal_i2c_e interface, uint32_t timeout_ms);

void mal_hspec_gnu_i2c_unlock_interface(mal_i2c_e interface);

#endif /* HSPEC_GNU_MAL_HSPEC_GNU_I2C_H_ */
