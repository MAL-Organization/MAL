/*
 * mal_hspec_mingw_i2c.h
 *
 *  Created on: Mar 24, 2016
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

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_I2C_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_I2C_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_mingw_get_valid_i2c_ios(mal_hspec_i2c_e interface, const mal_hspec_gpio_s **scls, uint8_t *scls_size, const mal_hspec_gpio_s **sdas, uint8_t *sdas_size);

mal_error_e mal_hspec_mingw_i2c_master_init(mal_hspec_i2c_init_s *init);

mal_error_e mal_hspec_mingw_i2c_transfer(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg);

mal_error_e mal_hspec_mingw_i2c_get_transfer_msg(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_I2C_H_ */
