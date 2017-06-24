/*
 * mal_hspec_stm32f072cb.h
 *
 *  Created on: May 3, 2015
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

#ifndef HSPEC_STM_MAL_HSPEC_STM32F072CB_H_
#define HSPEC_STM_MAL_HSPEC_STM32F072CB_H_

#include "hspec/stm/stm32f0/stm32f072/stm32f072cb/mal_hspec_stm32f072cb_can.h"
#include "hspec/stm/stm32f0/stm32f072/stm32f072cb/mal_hspec_stm32f072cb_flash.h"
#include "hspec/stm/stm32f0/stm32f072/stm32f072cb/mal_hspec_stm32f072cb_gpio.h"
#include "hspec/stm/stm32f0/stm32f072/stm32f072cb/mal_hspec_stm32f072cb_i2c.h"
#include "hspec/stm/stm32f0/stm32f072/stm32f072cb/mal_hspec_stm32f072cb_spi.h"
#include "hspec/stm/stm32f0/stm32f072/stm32f072cb/mal_hspec_stm32f072cb_timer.h"

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"

mal_error_e mal_hspec_stm32f072cb_get_port_afs(mal_hspec_gpio_port_e port,
											   const mal_hspec_stm32f0_af_e (**afs)[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE]);

#endif /* HSPEC_STM_MAL_HSPEC_STM32F072CB_H_ */
