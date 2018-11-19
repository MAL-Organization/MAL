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

#include "stm32f0/stm32f0xx_rcc.h"
#include "stm32f0/stm32f0xx_dac.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "dac/mal_dac.h"
#include "std/mal_defs.h"

mal_error_e mal_dac_init(mal_dac_init_s *init, mal_dac_s *handle) {
	static bool initialized = false;
	// Enable clock, all DACs are on port A for this MCU.
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	// Set GPIO
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Mode = GPIO_Mode_AN;
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->pin);
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->port), &gpio_init);
	// Reset DAC
	if (!initialized) {
		DAC_DeInit();
	}
	// Choose DAC channel
	uint32_t channel = DAC_Channel_1;
	if (MAL_DAC_1 != init->dac) {
		channel = DAC_Channel_2;
	}
	// Initialize DAC
	DAC_InitTypeDef dac_init;
	DAC_StructInit(&dac_init);
	dac_init.DAC_Trigger = DAC_Trigger_None;
	dac_init.DAC_WaveGeneration = DAC_WaveGeneration_None;
	dac_init.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	dac_init.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(channel, &dac_init);
	// Enable DAC
	DAC_Cmd(channel, ENABLE);
	// Flag that this was at least initialized once
	initialized = true;
	// Initialize handle
    switch (init->dac) {
        case MAL_DAC_0:
            handle->set_channel_data = &DAC_SetChannel1Data;
            break;
        case MAL_DAC_1:
            handle->set_channel_data = &DAC_SetChannel2Data;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }

	return MAL_ERROR_OK;
}

mal_error_e mal_dac_write_bits(mal_dac_s *handle, uint64_t value) {
    handle->set_channel_data(DAC_Align_12b_R, (uint16_t)value);
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
