/*
 * mal_hspec_stm32f072_dac.h
 *
 *  Created on: Oct 25, 2016
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

#ifndef HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_DAC_H_
#define HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_DAC_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_stm32f072_get_valid_dac_ios(mal_hspec_adc_e dac,
												  const mal_hspec_gpio_s **ios,
												  uint8_t *size);

#endif /* HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_DAC_H_ */