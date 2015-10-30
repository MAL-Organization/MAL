/*
 * mal_hspec_stm32f0_i2c.h
 *
 *  Created on: Jun 24, 2015
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_I2C_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_I2C_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec.h"
#include "stm32f0/stm32f0xx_misc.h"
#include "std/mal_bool.h"

#define mal_hspec_stm32f0_i2c_enable_interrupt(interface, active) do {\
	if (active) { \
		NVIC_EnableIRQ(mal_hspec_stm32f0_i2c_get_irq(interface)); \
	} \
} while(0)

mal_error_e mal_hspec_stm32f0_i2c_master_init(mal_hspec_i2c_init_s *init);

mal_error_e mal_hspec_stm32f0_i2c_transfer(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg);

IRQn_Type mal_hspec_stm32f0_i2c_get_irq(mal_hspec_i2c_e interface);

bool mal_hspec_stm32f0_i2c_disable_interrupt(mal_hspec_i2c_e interface);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_I2C_H_ */
