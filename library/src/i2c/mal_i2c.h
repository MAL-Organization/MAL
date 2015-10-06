/*
 * mal_i2c.h
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

#ifndef I2C_MAL_I2C_H_
#define I2C_MAL_I2C_H_

#include "gpio/mal_gpio.h"
#include "std/mal_error.h"
#include "hspec/mal_hspec.h"

#define mal_i2c_transfer(interface, msg) mal_hspec_i2c_transfer(interface, msg)

#define mal_i2c_disable_interrupt(interface) mal_hspec_i2c_disable_interrupt(interface)

#define mal_i2c_enable_interrupt(interface) mal_hspec_i2c_enable_interrupt(interface)

mal_error_e mal_i2c_init_master(mal_hspec_i2c_init_s *init);

#endif /* I2C_MAL_I2C_H_ */
