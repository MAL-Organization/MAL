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
static void mal_hspec_stm32f7_adc_handle_irq(mal_adc_s *adc);

static mal_adc_s *mal_hspec_stm32f7_adc_1 = NULL;
static mal_adc_s *mal_hspec_stm32f7_adc_2 = NULL;
static mal_adc_s *mal_hspec_stm32f7_adc_3 = NULL;

mal_error_e mal_adc_init(mal_adc_init_s *init, mal_adc_s *adc) {
    mal_error_e result;
    HAL_StatusTypeDef hal_result;
    // Enable ADC clock domain
    switch (init->adc) {
        case MAL_ADC_0:
            __HAL_RCC_ADC1_CLK_ENABLE();
            adc->hal_adc.Instance = ADC1;
            mal_hspec_stm32f7_adc_1 = adc;
            break;
        case MAL_ADC_1:
            __HAL_RCC_ADC2_CLK_ENABLE();
            adc->hal_adc.Instance = ADC2;
            mal_hspec_stm32f7_adc_2 = adc;
            break;
        case MAL_ADC_2:
            __HAL_RCC_ADC3_CLK_ENABLE();
            adc->hal_adc.Instance = ADC3;
            mal_hspec_stm32f7_adc_3 = adc;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    // Reset ADC
    hal_result = HAL_ADC_DeInit(&adc->hal_adc);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_INIT_FAILED;
    }
    // Initialise ADC
    // Get HAL resolution
    result = mal_hspec_stm32f7_adc_get_resolution(init->bit_resolution, &adc->hal_adc.Init.Resolution);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    adc->hal_adc.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
    // Only single conversions are supported for now
    adc->hal_adc.Init.ScanConvMode = DISABLE;
    adc->hal_adc.Init.ContinuousConvMode = DISABLE;
    adc->hal_adc.Init.DiscontinuousConvMode = DISABLE;
    adc->hal_adc.Init.NbrOfDiscConversion = 0;
    adc->hal_adc.Init.NbrOfConversion = 1;
    // No external triggers
    adc->hal_adc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc->hal_adc.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    adc->hal_adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc->hal_adc.Init.DMAContinuousRequests = DISABLE;
    adc->hal_adc.Init.EOCSelection = DISABLE;
    hal_result = HAL_ADC_Init(&adc->hal_adc);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_INIT_FAILED;
    }
    // Initialize handle
    adc->active = false;
    adc->resolution = init->bit_resolution;
    mal_circular_buffer_init(adc->async_channels,
                             sizeof(mal_adc_channel_s*),
                             sizeof(mal_adc_channel_s*) * MAL_HSPEC_STM32F7_ADC_CHANNEL_COUNT,
                             (mal_circular_buffer_s*)&adc->async_channels_buffer);

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

mal_error_e mal_adc_channel_init(mal_adc_channel_init_s *init, mal_adc_s *adc, mal_adc_channel_s *channel) {
    mal_error_e result;
    // Enable GPIO clock domain
    result = mal_hspec_stm32f7_gpio_init_analog(init->port, init->pin);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Prepare channel configuration
    channel->hal_channel_config.Channel = init->channel;
    channel->hal_channel_config.Rank = ADC_REGULAR_RANK_1;
    channel->hal_channel_config.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    channel->hal_channel_config.Offset = 0;
    // Set handle variables
    channel->adc = adc;
    channel->index = init->channel;

    return MAL_ERROR_OK;
}

mal_error_e mal_adc_read_bits(mal_adc_channel_s *channel, uint64_t *value) {
    HAL_StatusTypeDef hal_result;
    // Make sure adc is not active
    mal_error_e result = MAL_ERROR_OK;
    mal_adc_interrupt_state_s state;
    mal_adc_disable_interrupt(channel->adc, &state);
    if (channel->adc->active) {
        result = MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    mal_adc_restore_interrupt(channel->adc, &state);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    channel->adc->active = true;
    // Disable interrupt
    __HAL_ADC_DISABLE_IT(&channel->adc->hal_adc, (ADC_IT_EOC | ADC_IT_OVR));
    // Configure channel
    hal_result = HAL_ADC_ConfigChannel(&channel->adc->hal_adc, &channel->hal_channel_config);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    // Start conversion
    hal_result = HAL_ADC_Start(&channel->adc->hal_adc);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    // Wait end of conversion
    hal_result = HAL_ADC_PollForConversion(&channel->adc->hal_adc, 10);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    // Read value
    *value = HAL_ADC_GetValue(&channel->adc->hal_adc);

    return MAL_ERROR_OK;
}

mal_error_e mal_adc_resolution(mal_adc_s *adc, uint8_t *resolution) {
    *resolution = adc->resolution;
    return MAL_ERROR_OK;
}

mal_error_e mal_adc_async_read(mal_adc_channel_s *channel, mal_adc_read_callback_t callback, void *callback_handle) {
    // Make sure a conversion for that channel is not in progress
    mal_error_e result = MAL_ERROR_OK;
    mal_adc_interrupt_state_s state;
    mal_adc_disable_interrupt(channel->adc, &state);
    uint64_t buffer_index;
    for (buffer_index = 0; buffer_index < channel->adc->async_channels_buffer.size; buffer_index++) {
        mal_error_e peek_result;
        mal_adc_channel_s *buffered_channel;
        peek_result = mal_circular_buffer_peek((mal_circular_buffer_s*)&channel->adc->async_channels_buffer,
                                               buffer_index,
                                               &buffered_channel);
        if (MAL_ERROR_OK != peek_result) {
            result = peek_result;
            break;
        }
        if (buffered_channel == channel) {
            result = MAL_ERROR_HARDWARE_UNAVAILABLE;
            break;
        }
    }
    mal_adc_restore_interrupt(channel->adc, &state);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Save transfer
    mal_adc_disable_interrupt(channel->adc, &state);
    channel->callback = callback;
    channel->callback_handle = callback_handle;
    result = mal_circular_buffer_write((mal_circular_buffer_s*)&channel->adc->async_channels_buffer, &channel);
    mal_adc_restore_interrupt(channel->adc, &state);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Check if a new transfer should be started
    HAL_StatusTypeDef hal_result;
    mal_adc_disable_interrupt(channel->adc, &state);
    if (!channel->adc->active) {
        hal_result = HAL_ADC_ConfigChannel(&channel->adc->hal_adc, &channel->hal_channel_config);
        if (HAL_OK == hal_result) {
            hal_result = HAL_ADC_Start_IT(&channel->adc->hal_adc);
            if (HAL_OK == hal_result) {
                channel->adc->active = true;
                // Force interrupt to active
                state.active = true;
            } else {
                result = MAL_ERROR_HARDWARE_UNAVAILABLE;
            }
        }
    }
    mal_adc_restore_interrupt(channel->adc, &state);
    return result;
}

MAL_DEFS_INLINE void mal_adc_disable_interrupt(mal_adc_s *handle, mal_adc_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
    // Check UART IRQ
    state->active = NVIC_GetEnableIRQ(ADC_IRQn);
    NVIC_DisableIRQ(ADC_IRQn);
    __DSB();
    __ISB();
}

MAL_DEFS_INLINE void mal_adc_restore_interrupt(mal_adc_s *handle, mal_adc_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
    if (state->active) {
        NVIC_EnableIRQ(ADC_IRQn);
    }
}

mal_error_e mal_adc_maximum_resolution(mal_adc_e adc, uint8_t *resolution) {
    switch (adc) {
        case MAL_ADC_0:
        case MAL_ADC_1:
        case MAL_ADC_2:
            *resolution = 12;
            return MAL_ERROR_OK;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
}

void ADC_IRQHandler(void) {
    if (NULL != mal_hspec_stm32f7_adc_1) {
        HAL_ADC_IRQHandler(&mal_hspec_stm32f7_adc_1->hal_adc);
    }
    if (NULL != mal_hspec_stm32f7_adc_2) {
        HAL_ADC_IRQHandler(&mal_hspec_stm32f7_adc_2->hal_adc);
    }
    if (NULL != mal_hspec_stm32f7_adc_3) {
        HAL_ADC_IRQHandler(&mal_hspec_stm32f7_adc_3->hal_adc);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (NULL != mal_hspec_stm32f7_adc_1) {
        if (&mal_hspec_stm32f7_adc_1->hal_adc == hadc) {
            mal_hspec_stm32f7_adc_handle_irq(mal_hspec_stm32f7_adc_1);
        }
    }
    if (NULL != mal_hspec_stm32f7_adc_2) {
        if (&mal_hspec_stm32f7_adc_2->hal_adc == hadc) {
            mal_hspec_stm32f7_adc_handle_irq(mal_hspec_stm32f7_adc_2);
        }
    }
    if (NULL != mal_hspec_stm32f7_adc_3) {
        if (&mal_hspec_stm32f7_adc_3->hal_adc == hadc) {
            mal_hspec_stm32f7_adc_handle_irq(mal_hspec_stm32f7_adc_3);
        }
    }
}

static void mal_hspec_stm32f7_adc_handle_irq(mal_adc_s *adc) {
    mal_error_e result;
    // Get channel
    mal_adc_channel_s *channel;
    result = mal_circular_buffer_read((mal_circular_buffer_s*)&adc->async_channels_buffer, &channel);
    if (MAL_ERROR_OK != result) {
        adc->active = false;
        return;
    }
    // Execute callback
    uint32_t value = HAL_ADC_GetValue(&adc->hal_adc);
    channel->callback(channel->callback_handle, channel, value);
    // Start a new conversion if queued
    result = mal_circular_buffer_peek((mal_circular_buffer_s*)&adc->async_channels_buffer, 0, &channel);
    if (MAL_ERROR_OK != result) {
        adc->active = false;
        return;
    }
    HAL_StatusTypeDef hal_result;
    hal_result = HAL_ADC_ConfigChannel(&channel->adc->hal_adc, &channel->hal_channel_config);
    if (HAL_OK != hal_result) {
        adc->active = false;
        return;
    }
    hal_result = HAL_ADC_Start_IT(&channel->adc->hal_adc);
    if (HAL_OK != hal_result) {
        adc->active = false;
        return;
    }
}
