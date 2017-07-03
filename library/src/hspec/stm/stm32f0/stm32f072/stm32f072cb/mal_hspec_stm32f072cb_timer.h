/*
 * mal_hspec_stm32f072cb_timer.h
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

#ifndef HSPEC_STM_STM32F0_STM32F072_STM32F072CB_MAL_HSPEC_STM32F072CB_TIMER_H_
#define HSPEC_STM_STM32F0_STM32F072_STM32F072CB_MAL_HSPEC_STM32F072CB_TIMER_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"
#include "hspec/stm/stm32f0/mal_hspec_stm32f0_cmn.h"

mal_error_e mal_hspec_stm32f072cb_get_valid_input_capture_ios(mal_timer_e timer, const mal_hspec_gpio_s **ios, uint8_t *size);
void mal_hspec_stm32f072cb_get_timer_afs(const mal_hspec_stm32f0_af_e (**afs)[MAL_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_TIMER_SIZE]);

#endif /* HSPEC_STM_STM32F0_STM32F072_STM32F072CB_MAL_HSPEC_STM32F072CB_TIMER_H_ */
