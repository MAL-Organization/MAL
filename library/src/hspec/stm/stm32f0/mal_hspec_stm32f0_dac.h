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

#ifndef MAL_MAL_HSPEC_STM32F0_DAC_H
#define MAL_MAL_HSPEC_STM32F0_DAC_H

#include "std/mal_stdint.h"

typedef void (*mal_hspec_stm32f0_dac_set_channel_data_t)(uint32_t dac_align, uint16_t data);

typedef struct MAL_DAC {
    mal_hspec_stm32f0_dac_set_channel_data_t set_channel_data;
} mal_dac_s;

#endif //MAL_MAL_HSPEC_STM32F0_DAC_H
