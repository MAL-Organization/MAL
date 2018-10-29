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

#include "mal_hspec_stm32f7_dac.h"
#include "dac/mal_dac.h"
#include "stm32f7/stm32f7xx_hal_rcc.h"

static DAC_HandleTypeDef mal_hspec_stm32f7_dac;

mal_error_e mal_dac_init(mal_dac_init_s *init, mal_dac_s *handle) {
    HAL_StatusTypeDef hal_result;
    mal_error_e result;
    static bool initialized = false;
    // Enable clock
    __HAL_RCC_DAC_CLK_ENABLE();
    mal_hspec_stm32f7_dac.Instance = DAC;
    // Set GPIO
    result = mal_hspec_stm32f7_gpio_init_analog(init->port, init->pin);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Reset DAC
    if (!initialized) {
        hal_result = HAL_DAC_DeInit(&mal_hspec_stm32f7_dac);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_INIT_FAILED;
        }
    }
    // Choose DAC channel
    uint32_t channel = DAC_CHANNEL_1;
    if (MAL_DAC_1 != init->dac) {
        channel = DAC_CHANNEL_2;
    }
    // Initialize DAC
    if (!initialized) {
        hal_result = HAL_DAC_Init(&mal_hspec_stm32f7_dac);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_INIT_FAILED;
        }
        hal_result = HAL_DAC_Start(&mal_hspec_stm32f7_dac, channel);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_INIT_FAILED;
        }
    }
    // Flag that this was at least initialized once
    initialized = true;
    // Configure channel
    DAC_ChannelConfTypeDef channel_init;
    channel_init.DAC_Trigger = DAC_TRIGGER_NONE;
    channel_init.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    hal_result = HAL_DAC_ConfigChannel(&mal_hspec_stm32f7_dac, &channel_init, channel);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_INIT_FAILED;
    }
    // Initialize handle
    handle->hal_dac = &mal_hspec_stm32f7_dac;
    handle->channel = channel;

    return MAL_ERROR_OK;
}

mal_error_e mal_dac_write_bits(mal_dac_s *handle, uint64_t value) {
    HAL_StatusTypeDef hal_result;
    hal_result = HAL_DAC_SetValue(handle->hal_dac, handle->channel, DAC_ALIGN_12B_R, (uint32_t)value);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_OPERATION_INVALID;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_dac_resolution(mal_dac_s *handle, uint8_t *resolution) {
    MAL_DEFS_UNUSED(handle);
    *resolution = 12;
    return MAL_ERROR_OK;
}

mal_error_e mal_dac_maximum_resolution(mal_dac_e dac, uint8_t *resolution) {
    if (dac > MAL_DAC_1) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    *resolution = 12;
    return MAL_ERROR_OK;
}
