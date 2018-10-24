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

#include "mal_hspec_stm32f7_adc.h"
#include "stm32f7/stm32f7xx_hal_rcc.h"

static mal_error_e mal_hspec_stm32f7_adc_get_resolution(uint8_t bit_resolution, uint32_t *resolution);

static ADC_HandleTypeDef mal_hspec_stm32f_adc_1;
static ADC_HandleTypeDef mal_hspec_stm32f_adc_2;
static ADC_HandleTypeDef mal_hspec_stm32f_adc_3;

mal_error_e mal_adc_init(mal_adc_init_s *init, mal_adc_s *handle) {
    static bool adc1_initialised = false;
    static bool adc2_initialised = false;
    static bool adc3_initialised = false;
    bool *initialised;
    mal_error_e result;
    HAL_StatusTypeDef hal_result;
    // Enable ADC clock domain
    uint32_t adc_index;
    result = mal_adc_get_adc_from_enum(init->adc, &adc_index);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    switch (adc_index) {
        case 0:
            if (!adc1_initialised) {
                __HAL_RCC_ADC1_CLK_ENABLE();
                mal_hspec_stm32f_adc_1.Instance = ADC1;
            }
            handle->hal_adc = &mal_hspec_stm32f_adc_1;
            initialised = &adc1_initialised;
            break;
        case 1:
            if (!adc2_initialised) {
                __HAL_RCC_ADC2_CLK_ENABLE();
                mal_hspec_stm32f_adc_2.Instance = ADC2;
            }
            handle->hal_adc = &mal_hspec_stm32f_adc_2;
            initialised = &adc2_initialised;
            break;
        case 3:
            if (!adc3_initialised) {
                __HAL_RCC_ADC3_CLK_ENABLE();
                mal_hspec_stm32f_adc_3.Instance = ADC3;
            }
            handle->hal_adc = &mal_hspec_stm32f_adc_3;
            initialised = &adc3_initialised;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    // Enable GPIO clock domain
    result = mal_hspec_stm32f7_gpio_init_analog(init->port, init->pin);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Reset ADC
    if (!(*initialised)) {
        hal_result = HAL_ADC_DeInit(handle->hal_adc);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_INIT_FAILED;
        }
    }
    // Initialise ADC
    // Get HAL resolution
    result = mal_hspec_stm32f7_adc_get_resolution(init->bit_resolution, &handle->hal_adc->Init.Resolution);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    if (!(*initialised)) {
        handle->hal_adc->Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
        // Only single conversions are supported for now
        handle->hal_adc->Init.ScanConvMode = DISABLE;
        handle->hal_adc->Init.ContinuousConvMode = DISABLE;
        handle->hal_adc->Init.DiscontinuousConvMode = DISABLE;
        handle->hal_adc->Init.NbrOfDiscConversion = 0;
        handle->hal_adc->Init.NbrOfConversion = 1;
        // No external triggers
        handle->hal_adc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        handle->hal_adc->Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
        handle->hal_adc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
        handle->hal_adc->Init.DMAContinuousRequests = DISABLE;
        handle->hal_adc->Init.EOCSelection = DISABLE;
        hal_result = HAL_ADC_Init(handle->hal_adc);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_INIT_FAILED;
        }
    } else {
        // Check resolution
        if ((handle->hal_adc->Instance->CR1 & ADC_CR1_RES_Msk) != handle->hal_adc->Init.Resolution) {
            return MAL_ERROR_HARDWARE_UNAVAILABLE;
        }
    }
    *initialised = true;
    // Prepare channel configuration
    result = mal_adc_
    handle->hal_channel_config.Channel = ADC_CHANNEL_10;
    // Initialize handle
    handle->adc_channel = (uint32_t)1 << init->adc;
    handle->resolution = init->bit_resolution;
    // Save handle
    adc_handles[init->adc] = handle;

    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f7_adc_get_resolution(uint8_t bit_resolution, uint32_t *resolution) {
    switch (bit_resolution) {
        case 12:
            *resolution = ADC_RESOLUTION_12B;
            return MAL_ERROR_OK;
        case 10:
            *resolution = ADC_RESOLUTION_10B;
            return MAL_ERROR_OK;
        case 8:
            *resolution = ADC_RESOLUTION_8B;
            return MAL_ERROR_OK;
        case 6:
            *resolution = ADC_RESOLUTION_6B;
            return MAL_ERROR_OK;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
}
