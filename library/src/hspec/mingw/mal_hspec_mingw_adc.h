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

#include "adc/mal_adc.h"

void mal_hspec_mingw_adc_set_value(mal_adc_e adc, float value);

void mal_hspec_mingw_adc_do_async(mal_adc_e adc);

bool mal_hspec_mingw_adc_peek_async(mal_adc_e adc);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_ADC_H_ */
