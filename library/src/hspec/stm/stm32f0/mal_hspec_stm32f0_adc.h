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
#include "utils/mal_circular_buffer.h"
#include "std/mal_bool.h"

#define MAL_HSPEC_STM32F0_ADC_CHANNEL_COUNT 16

typedef struct MAL_ADC {
    uint8_t resolution;
    bool active;
    volatile mal_circular_buffer_s async_channels_buffer;
    mal_adc_channel_s *async_channels[MAL_HSPEC_STM32F0_ADC_CHANNEL_COUNT];
} mal_adc_s;

typedef struct MAL_ADC_CHANNEL {
    mal_adc_s *adc;
    uint32_t channel;
    mal_adc_read_callback_t callback;
    void *callback_handle;
} mal_adc_channel_s;

typedef struct MAL_ADC_INTERRUPT_STATE {
    bool active;
} mal_adc_interrupt_state_s;

#endif //MAL_MAL_HSPEC_STM32F0_ADC_H
