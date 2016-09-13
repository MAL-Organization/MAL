/*
 * mal_hspec_stm32f072cb_i2c.h
 *
 *  Created on: Sep 7, 2016
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

#ifndef HSPEC_STM_STM32F0_STM32F072_STM32F072CB_MAL_HSPEC_STM32F072CB_I2C_H_
#define HSPEC_STM_STM32F0_STM32F072_STM32F072CB_MAL_HSPEC_STM32F072CB_I2C_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_stm32f072cb_get_valid_i2c_ios(mal_hspec_i2c_e interface,
													const mal_hspec_gpio_s **scls,
													uint8_t *scls_size,
													const mal_hspec_gpio_s **sdas,
													uint8_t *sdas_size);

#endif /* HSPEC_STM_STM32F0_STM32F072_STM32F072CB_MAL_HSPEC_STM32F072CB_I2C_H_ */
