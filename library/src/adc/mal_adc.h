/*
 * mal_adc.h
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

#ifndef ADC_MAL_ADC_H_
#define ADC_MAL_ADC_H_

#include "hspec/mal_hspec.h"

#define mal_adc_read_bits(adc, value) mal_hspec_adc_read(adc, value)

#define mal_adc_resolution(adc, resolution) mal_hspec_adc_resolution(adc, resolution)

#define mal_adc_async_read(adc, callback) mal_hspec_adc_async_read(adc, callback)

mal_error_e mal_adc_init(mal_hspec_adc_init_s *init);

mal_error_e mal_adc_read_volts(mal_hspec_adc_e adc, float *value);

mal_error_e mal_adc_bits_to_volts(mal_hspec_adc_e adc, uint64_t bit_value, float *value);

#endif /* ADC_MAL_ADC_H_ */
