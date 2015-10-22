/*
 * mal_hspec_stm32f0_adc.h
 *
 *  Created on: Sep 11, 2015
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_ADC_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_ADC_H_

#include "hspec/mal_hspec.h"

mal_error_e mal_hspec_stm32f0_adc_init(mal_hspec_adc_init_s *init);
mal_error_e mal_hspec_stm32f0_adc_read(mal_hspec_adc_e adc, uint64_t *value);
mal_error_e mal_hspec_stm32f0_adc_resolution(mal_hspec_adc_e adc, uint8_t *resolution);
mal_error_e mal_hspec_stm32f0_adc_async_read(mal_hspec_adc_e adc, mal_hspec_adc_read_callback_t callback);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_ADC_H_ */