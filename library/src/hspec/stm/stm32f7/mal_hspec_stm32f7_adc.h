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

#ifndef MAL_MAL_HSPEC_STM32F7_ADC_H
#define MAL_MAL_HSPEC_STM32F7_ADC_H

#include "adc/mal_adc.h"
#include "stm32f7/stm32f7xx_hal_adc.h"
#include "std/mal_bool.h"
#include "utils/mal_circular_buffer.h"

#define MAL_HSPEC_STM32F7_ADC_CHANNEL_COUNT 16

typedef struct MAL_ADC {
    volatile bool active;
    ADC_HandleTypeDef hal_adc;
    uint8_t resolution;
    volatile mal_circular_buffer_s async_channels_buffer;
    mal_adc_channel_s *async_channels[MAL_HSPEC_STM32F7_ADC_CHANNEL_COUNT];
} mal_adc_s;

typedef struct MAL_ADC_CHANNEL {
    mal_adc_s *adc;
    ADC_ChannelConfTypeDef hal_channel_config;
    uint8_t index;
    mal_adc_read_callback_t callback;
    void *callback_handle;
} mal_adc_channel_s;

typedef struct MAL_ADC_INTERRUPT_STATE {
    uint32_t active;
} mal_adc_interrupt_state_s;

#endif //MAL_MAL_HSPEC_STM32F7_ADC_H
