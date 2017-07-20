/*
 * test_mal_adc.h
 *
 *  Created on: Jan 30, 2017
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

#ifndef ADC_TEST_MAL_ADC_H_
#define ADC_TEST_MAL_ADC_H_

#include "std/mal_error.h"
#include "adc/mal_adc.h"
#include "gpio/mal_gpio.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_get_valid_adc_ios(mal_adc_e adc, const mal_gpio_s **ios, uint8_t *size);

void test_mal_adc(void);

#endif /* ADC_TEST_MAL_ADC_H_ */
