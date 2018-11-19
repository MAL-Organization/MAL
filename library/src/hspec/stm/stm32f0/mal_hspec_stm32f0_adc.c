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

#include "adc/mal_adc.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "stm32f0/stm32f0xx_gpio.h"
#include "stm32f0/stm32f0xx_adc.h"
#include "std/mal_bool.h"
#include "std/mal_stdlib.h"
#include "std/mal_defs.h"

static mal_error_e mal_hspec_stm32f0_adc_get_resolution(uint8_t bit_resolution, uint32_t *resolution);
static void mal_hspec_stm32f0_adc_start_conversion(mal_adc_channel_s *channel);

static mal_adc_s *mal_hspec_stm32f0_adc_1 = NULL;

mal_error_e mal_adc_init(mal_adc_init_s *init, mal_adc_s *adc) {
	ADC_InitTypeDef adc_init;
	mal_error_e result;
	if (MAL_ADC_0 != init->adc) {
        return MAL_ERROR_HARDWARE_INVALID;
	}
	// Enable ADC clock domain
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	// Reset ADC
    ADC_DeInit(ADC1);
	// Configure ADC
	ADC_StructInit(&adc_init);
	// Set resolution
	result = mal_hspec_stm32f0_adc_get_resolution(init->bit_resolution, &adc_init.ADC_Resolution);
	if (MAL_ERROR_OK != result) {
		return result;
	}
    // Only single conversions are supported for now
    adc_init.ADC_ContinuousConvMode = DISABLE;
    // No external triggers
    adc_init.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
    adc_init.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_Init(ADC1, &adc_init);
	// Calibrate ADC
    ADC_GetCalibrationFactor(ADC1);
	// Enable ADC
    ADC_Cmd(ADC1, ENABLE);
	// Initialize handle
    adc->resolution = init->bit_resolution;
    adc->active = false;
    mal_circular_buffer_init(adc->async_channels,
                             sizeof(mal_adc_channel_s*),
                             sizeof(mal_adc_channel_s*) * MAL_HSPEC_STM32F0_ADC_CHANNEL_COUNT,
                             (mal_circular_buffer_s*)&adc->async_channels_buffer);
	// Save handle
    mal_hspec_stm32f0_adc_1 = adc;

	return MAL_ERROR_OK;
}

mal_error_e mal_adc_channel_init(mal_adc_channel_init_s *init, mal_adc_s *adc, mal_adc_channel_s *channel) {
    if (init->channel >= MAL_HSPEC_STM32F0_ADC_CHANNEL_COUNT) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    // Set channel
    channel->adc = adc;
    channel->channel = (uint32_t)1 << (uint32_t)init->channel;
    // Enable GPIO clock domain
    RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->port), ENABLE);
    // Set GPIO
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Mode = GPIO_Mode_AN;
    gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->pin);
    gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->port), &gpio_init);

    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f0_adc_get_resolution(uint8_t bit_resolution, uint32_t *resolution) {
	switch (bit_resolution) {
		case 12:
			*resolution = ADC_Resolution_12b;
			return MAL_ERROR_OK;
		case 10:
			*resolution = ADC_Resolution_10b;
			return MAL_ERROR_OK;
		case 8:
			*resolution = ADC_Resolution_8b;
			return MAL_ERROR_OK;
		case 6:
			*resolution = ADC_Resolution_6b;
			return MAL_ERROR_OK;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}
}

mal_error_e mal_adc_read_bits(mal_adc_channel_s *channel, uint64_t *value) {
    // Make sure adc is not running
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
	// Disable interrupt
    // 12 equates to ADC_IRQ. However, the name of the constant changes based
    // on the MCU because it is multiplex with other interrupts in some of them. It
    // is simpler to use the constant directly.
	NVIC_DisableIRQ((IRQn_Type)12);
	ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);
	// Start conversion
    mal_hspec_stm32f0_adc_start_conversion(channel);
	// Wait end of conversion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	// Read value
	*value = ADC_GetConversionValue(ADC1);

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
    mal_adc_disable_interrupt(channel->adc, &state);
    if (!channel->adc->active) {
        // Configure interrupt
        // 12 equates to ADC_IRQ. However, the name of the constant changes based
        // on the MCU because it is multiplex with other interrupts in some of them. It
        // is simpler to use the constant directly.
        NVIC_EnableIRQ((IRQn_Type)12);
        ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
        // Start conversion
        mal_hspec_stm32f0_adc_start_conversion(channel);
        state.active = true;
    }
    mal_adc_restore_interrupt(channel->adc, &state);

	return MAL_ERROR_OK;
}

static void mal_hspec_stm32f0_adc_start_conversion(mal_adc_channel_s *channel) {
	// Clear ADC configuration
	ADC1->CHSELR = 0;
	// Configure channel
	ADC_ChannelConfig(ADC1, channel->channel, ADC_SampleTime_239_5Cycles);
	// Wait for ADC to be ready
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY) == RESET);
	// Start conversion
	ADC_StartOfConversion(ADC1);
}

mal_error_e mal_adc_resolution(mal_adc_s *handle, uint8_t *resolution) {
	*resolution = handle->resolution;
    return MAL_ERROR_OK;
}

void ADC1_COMP_IRQHandler(void) {
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
    if (NULL == mal_hspec_stm32f0_adc_1) {
        return;
    }
    mal_adc_s *adc = mal_hspec_stm32f0_adc_1;
    // Get channel
    mal_error_e result;
    mal_adc_channel_s *channel;
    result = mal_circular_buffer_read((mal_circular_buffer_s*)&adc->async_channels_buffer, &channel);
    if (MAL_ERROR_OK != result) {
        adc->active = false;
        return;
    }
    // Execute callback
    uint16_t value = ADC_GetConversionValue(ADC1);
    channel->callback(channel->callback_handle, channel, value);
    // Start a new conversion if queued
    result = mal_circular_buffer_peek((mal_circular_buffer_s*)&adc->async_channels_buffer, 0, channel);
    if (MAL_ERROR_OK != result) {
        adc->active = false;
        return;
    }
    mal_hspec_stm32f0_adc_start_conversion(channel);
}

MAL_DEFS_INLINE void mal_adc_disable_interrupt(mal_adc_s *handle, mal_adc_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
	// 12 equates to ADC_IRQ. However, the name of the constant changes based
	// on the MCU because it is multiplex with other interrupts in some of them. It
	// is simpler to use the constant directly.
	state->active = NVIC_GetActive((IRQn_Type)12);
	// Enable interrupts
	NVIC_DisableIRQ((IRQn_Type)12);
	__DSB();
	__ISB();
}

MAL_DEFS_INLINE void mal_adc_restore_interrupt(mal_adc_s *handle, mal_adc_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
	if (state->active) {
		NVIC_EnableIRQ((IRQn_Type)12);
	}
}

mal_error_e mal_adc_maximum_resolution(mal_adc_e adc, uint8_t *resolution) {
	switch (adc) {
		case MAL_ADC_0:
			*resolution = 12;
			return MAL_ERROR_OK;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}
}
