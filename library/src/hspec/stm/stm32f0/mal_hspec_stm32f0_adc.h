/*
 * Copyright (c) 2018 Olivier Allaire
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

#ifndef MAL_MAL_HSPEC_STM32F0_ADC_H
#define MAL_MAL_HSPEC_STM32F0_ADC_H

#include "std/mal_stdint.h"
#include "adc/mal_adc.h"

typedef struct MAL_ADC {
    uint32_t adc_channel;
    uint8_t  resolution;
    mal_adc_read_callback_t callback;
    void *callback_handle;
} mal_adc_s;

#endif //MAL_MAL_HSPEC_STM32F0_ADC_H
