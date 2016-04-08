/*
 * mal_hspec_mingw_adc.h
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

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_ADC_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_ADC_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"
#include "std/mal_bool.h"

mal_error_e mal_hspec_mingw_get_valid_adc_ios(mal_hspec_adc_e adc,const mal_hspec_gpio_s **ios, uint8_t *size);

mal_error_e mal_hspec_mingw_adc_init(mal_hspec_adc_init_s *init);

mal_error_e mal_hspec_mingw_adc_read(mal_error_e adc, uint64_t *value);

mal_error_e mal_hspec_mingw_adc_resolution(mal_hspec_adc_e adc, uint8_t *resolution);

void mal_hspec_mingw_adc_set_value(mal_hspec_adc_e adc, float value);

mal_error_e mal_hspec_mingw_adc_async_read(mal_hspec_adc_e adc, mal_hspec_adc_read_callback_t callback);

void mal_hspec_mingw_adc_do_async(mal_hspec_adc_e adc);

bool mal_hspec_mingw_adc_peek_async(mal_hspec_adc_e adc);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_ADC_H_ */
