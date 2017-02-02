/*
 * mal_hspec_stm32f0_adc.c
 *
 *  Created on: Sep 11, 2015
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

#include "mal_hspec_stm32f0_adc.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "stm32f0/stm32f0xx_gpio.h"
#include "stm32f0/stm32f0xx_adc.h"
#include "std/mal_bool.h"
#include "std/mal_stdlib.h"

static mal_error_e get_adc_resolution(uint8_t bit_resolution, uint32_t *resolution);
static void start_adc_conversion(mal_hspec_adc_e adc);

static mal_hspec_adc_read_callback_t adc_callbacks[MAL_HSPEC_ADC_SIZE];

mal_error_e mal_hspec_stm32f0_adc_init(mal_hspec_adc_init_s *init) {
	static bool initialised = false;
	ADC_InitTypeDef adc_init;
	mal_error_e result;
	// Enable GPIO clock domain
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->gpio->port), ENABLE);
	// Enable ADC clock domain
	if (!initialised) {
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	}
	// Set GPIO
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Mode = GPIO_Mode_AN;
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->gpio->pin);
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->gpio->port), &gpio_init);
	// Reset ADC
	if (!initialised) {
		ADC_DeInit(ADC1);
	}
	// Configure ADC
	ADC_StructInit(&adc_init);
	// Set resolution
	result = get_adc_resolution(init->bit_resolution, &adc_init.ADC_Resolution);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	if (!initialised) {
		// Only single conversions are supported for now
		adc_init.ADC_ContinuousConvMode = DISABLE;
		// No external triggers
		adc_init.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
		adc_init.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_Init(ADC1, &adc_init);
	} else {
		// Check resolution
		if ((ADC1->CFGR1 & ADC_CFGR1_RES) != adc_init.ADC_Resolution) {
			return MAL_ERROR_HARDWARE_UNAVAILABLE;
		}
	}
	// Calibrate ADC
	if (!initialised) {
		ADC_GetCalibrationFactor(ADC1);
	}
	// Enable ADC
	if (!initialised) {
		ADC_Cmd(ADC1, ENABLE);
	}

	initialised = true;
	return MAL_ERROR_OK;
}

static mal_error_e get_adc_resolution(uint8_t bit_resolution, uint32_t *resolution) {
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

mal_error_e mal_hspec_stm32f0_adc_read(mal_hspec_adc_e adc, uint64_t *value) {
	// Disable interrupt
	NVIC_DisableIRQ(ADC1_COMP_IRQn);
	ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);
	// Start conversion
	start_adc_conversion(adc);
	// Wait end of conversion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	// Read value
	*value = ADC_GetConversionValue(ADC1);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_adc_async_read(mal_hspec_adc_e adc, mal_hspec_adc_read_callback_t callback) {
	NVIC_InitTypeDef nvic_init;
	// Save callback
	adc_callbacks[adc] = callback;
	// Configure interrupt
	// 12 equates to ADC_IRQ. However, the name of the constant changes based
	// on the MCU because it is multiplex with other interrupts in some of them. It
	// is simpler to use the constant directly.
	NVIC_EnableIRQ(12);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	// Start conversion
	start_adc_conversion(adc);

	return MAL_ERROR_OK;
}

static void start_adc_conversion(mal_hspec_adc_e adc) {
	// Clear ADC configuration
	ADC1->CHSELR = 0;
	// Configure channel
	ADC_ChannelConfig(ADC1, 1 << adc, ADC_SampleTime_239_5Cycles);
	// Wait for ADC to be ready
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY) == RESET);
	// Start conversion
	ADC_StartOfConversion(ADC1);
}

mal_error_e mal_hspec_stm32f0_adc_resolution(mal_hspec_adc_e adc, uint8_t *resolution) {
	switch (ADC1->CFGR1 & ADC_CFGR1_RES) {
	case ADC_Resolution_12b:
		*resolution = 12;
		return MAL_ERROR_OK;
	case ADC_Resolution_10b:
		*resolution = 10;
		return MAL_ERROR_OK;
	case ADC_Resolution_8b:
		*resolution = 8;
		return MAL_ERROR_OK;
	case ADC_Resolution_6b:
		*resolution = 6;
		return MAL_ERROR_OK;
	default:
		return MAL_ERROR_HARDWARE_INVALID;
	}
}

void ADC1_COMP_IRQHandler(void) {
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	// Get ADC
	mal_hspec_adc_e adc;
	switch (ADC1->CHSELR) {
		case 0x1:
			adc = MAL_HSPEC_ADC_0;
			break;
		case 0x2:
			adc = MAL_HSPEC_ADC_1;
			break;
		case 0x4:
			adc = MAL_HSPEC_ADC_2;
			break;
		case 0x8:
			adc = MAL_HSPEC_ADC_3;
			break;
		case 0x10:
			adc = MAL_HSPEC_ADC_4;
			break;
		case 0x20:
			adc = MAL_HSPEC_ADC_5;
			break;
		case 0x40:
			adc = MAL_HSPEC_ADC_6;
			break;
		case 0x80:
			adc = MAL_HSPEC_ADC_7;
			break;
		case 0x100:
			adc = MAL_HSPEC_ADC_8;
			break;
		case 0x200:
			adc = MAL_HSPEC_ADC_9;
			break;
		case 0x400:
			adc = MAL_HSPEC_ADC_10;
			break;
		case 0x800:
			adc = MAL_HSPEC_ADC_11;
			break;
		case 0x1000:
			adc = MAL_HSPEC_ADC_12;
			break;
		case 0x2000:
			adc = MAL_HSPEC_ADC_13;
			break;
		case 0x4000:
			adc = MAL_HSPEC_ADC_14;
			break;
		case 0x8000:
			adc = MAL_HSPEC_ADC_15;
			break;
		default:
			return;
	}
	// Execute callback
	mal_hspec_adc_read_callback_t cb = adc_callbacks[adc];
	adc_callbacks[adc] = NULL;
	if (cb != NULL) {
		cb(adc, ADC_GetConversionValue(ADC1));
	}
}

bool mal_hspec_stm32f0_disable_adc_interrupt(mal_hspec_adc_e adc) {
	// 12 equates to ADC_IRQ. However, the name of the constant changes based
	// on the MCU because it is multiplex with other interrupts in some of them. It
	// is simpler to use the constant directly.
	bool active = NVIC_GetActive(12);
	// Enable interrupts
	NVIC_DisableIRQ(12);
	__DSB();
	__ISB();

	return active;
}

mal_error_e mal_hspec_stm32f0_adc_maximum_resolution(mal_hspec_adc_e adc, uint8_t *resolution) {
	switch (adc) {
		case MAL_HSPEC_ADC_0:
		case MAL_HSPEC_ADC_1:
		case MAL_HSPEC_ADC_2:
		case MAL_HSPEC_ADC_3:
		case MAL_HSPEC_ADC_4:
		case MAL_HSPEC_ADC_5:
		case MAL_HSPEC_ADC_6:
		case MAL_HSPEC_ADC_7:
		case MAL_HSPEC_ADC_8:
		case MAL_HSPEC_ADC_9:
		case MAL_HSPEC_ADC_10:
		case MAL_HSPEC_ADC_11:
		case MAL_HSPEC_ADC_12:
		case MAL_HSPEC_ADC_13:
		case MAL_HSPEC_ADC_14:
		case MAL_HSPEC_ADC_15:
			*resolution = 12;
			return MAL_ERROR_OK;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}
}
