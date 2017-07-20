/*
 * mal_hspec_stm32.c
 *
 *  Created on: May 3, 2015
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

#include <stdbool.h>

#include "mal_hspec_stm32f0.h"
#include "std/mal_stdint.h"
#include "std/mal_math.h"
#include "mal_hspec_stm32f0_cmn.h"

#include "stm32f0/stm32f0xx_gpio.h"
#include "stm32f0/system_stm32f0xx.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "stm32f0/stm32f0xx.h"
#include "stm32f0/stm32f0xx_flash.h"

#include "cortexm/exception_handlers.h"

// Code from newlib...
// defined in linker script
extern unsigned int _sidata;
// Begin address for the .data section; defined in linker script
extern unsigned int _sdata;
// End address for the .data section; defined in linker script
extern unsigned int _edata;

// Begin address for the .bss section; defined in linker script
extern unsigned int __bss_start__;
// End address for the .bss section; defined in linker script
extern unsigned int __bss_end__;

#ifdef MAL_SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif

static void initialise_memory(void);

static const uint32_t hse_prediv_values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

static const uint32_t hse_prediv_reg_values[] = { RCC_CFGR2_PREDIV1_DIV1,
RCC_CFGR2_PREDIV1_DIV2, RCC_CFGR2_PREDIV1_DIV3, RCC_CFGR2_PREDIV1_DIV4,
RCC_CFGR2_PREDIV1_DIV5, RCC_CFGR2_PREDIV1_DIV6, RCC_CFGR2_PREDIV1_DIV7,
RCC_CFGR2_PREDIV1_DIV8, RCC_CFGR2_PREDIV1_DIV9, RCC_CFGR2_PREDIV1_DIV10,
RCC_CFGR2_PREDIV1_DIV11, RCC_CFGR2_PREDIV1_DIV12,
RCC_CFGR2_PREDIV1_DIV13, RCC_CFGR2_PREDIV1_DIV14,
RCC_CFGR2_PREDIV1_DIV15, RCC_CFGR2_PREDIV1_DIV16 };

static const uint32_t pll_mul_values[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

static const uint32_t pll_mul_reg_values[] = { RCC_CFGR_PLLMULL2,
RCC_CFGR_PLLMULL3, RCC_CFGR_PLLMULL4, RCC_CFGR_PLLMULL5,
RCC_CFGR_PLLMULL6, RCC_CFGR_PLLMULL7, RCC_CFGR_PLLMULL8,
RCC_CFGR_PLLMULL9, RCC_CFGR_PLLMULL10, RCC_CFGR_PLLMULL11,
RCC_CFGR_PLLMULL12, RCC_CFGR_PLLMULL13, RCC_CFGR_PLLMULL14,
RCC_CFGR_PLLMULL15, RCC_CFGR_PLLMULL16 };

extern uint64_t mal_external_clk_freq;
void mal_stm32f0_relocate_vector_table(void);

void mal_startup_hardware(void) {
	// Initialises basic clocks and systems
	SystemInit();

	initialise_memory();
	mal_clock_initialise_system_clock();

	// Update system core clock for stm32f0-stdperiph drivers
	SystemCoreClockUpdate(mal_external_clk_freq);

	// Relocate vector table
	mal_stm32f0_relocate_vector_table();

#ifdef MAL_SEMIHOSTING
	// Initialize monitor handles for semihosting
	initialise_monitor_handles();
#endif
}

mal_error_e mal_clock_set_system_clock_unmanaged(const mal_system_clk_s *clk) {
	uint32_t timeout_counter;
	uint32_t hserdy_status;
	uint64_t src_clk_freq;
	mal_system_clk_src_e clk_src = clk->src;
	// This MCU does support floats natively, so frequency is in millihertz
	uint32_t requested_frequency = clk->frequency / 1000;
	uint32_t target_frequency = requested_frequency;
	// Choose clk source
	switch (clk->src) {
	case MAL_SYSTEM_CLK_SRC_AUTO:
		// Since external clock is more precise than internal, MAL will favor
		// external in this case.
	case MAL_SYSTEM_CLK_SRC_EXTERNAL:
		// Check if there is an external source
		clk_src = MAL_SYSTEM_CLK_SRC_EXTERNAL;
		src_clk_freq = mal_clock_get_external_clock_frequency() / 1000;
		if (0 == src_clk_freq) {
			// No external, switch to internal
			src_clk_freq = HSI_VALUE;
			clk_src = MAL_SYSTEM_CLK_SRC_INTERNAL;
		}
		break;
	case MAL_SYSTEM_CLK_SRC_INTERNAL:
	default:
		// Set HSI on
		src_clk_freq = HSI_VALUE;
		break;
	}
	// Start HSE clock if source
	if (MAL_SYSTEM_CLK_SRC_EXTERNAL == clk_src) {
		RCC_HSEConfig(RCC_HSE_ON);
		if (SUCCESS != RCC_WaitForHSEStartUp()) {
			// HSE startup was unsuccessful, use HSI
			clk_src = MAL_SYSTEM_CLK_SRC_INTERNAL;
			src_clk_freq = HSI_VALUE;
		}
	}
	// Set flash prefetch buffer before changing clock
	FLASH_PrefetchBufferCmd(ENABLE);
	// Check if clock can be used without PLL
	if (target_frequency == src_clk_freq) {
		// Set flash latency before switching
		if (src_clk_freq > 24000000) {
			FLASH_SetLatency(FLASH_Latency_1);
		}
		// Set SW
		if (MAL_SYSTEM_CLK_SRC_INTERNAL == clk_src) {
			RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
		} else {
			RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
		}
	} else {
		// Cap target frequency
		if (target_frequency > MAL_HSPEC_STM32F0_MAX_FREQUENCY) {
			target_frequency = MAL_HSPEC_STM32F0_MAX_FREQUENCY;
		}
		// Select PLL input clock
		uint64_t pll_src_clock;
		if (clk_src == MAL_SYSTEM_CLK_SRC_INTERNAL) {
			pll_src_clock = HSI_VALUE / MAL_HSPEC_STM32F0_HSI_PLL_DIV;
		} else {
			pll_src_clock = src_clk_freq;
		}
		// PLL required, must select divider and multiplier
		uint64_t freq_diff = UINT64_MAX;
		uint32_t divider = 0;
		uint64_t multiplier = 0;
		int i, j;
		for (i = 0; i < (sizeof(hse_prediv_values) / sizeof(uint32_t)); i++) {
			for (j = 0; j < (sizeof(pll_mul_values) / sizeof(uint32_t)); j++) {
				// Compute resulting system frequency
				uint64_t sys_freq;
				if (mal_hspec_stm32f0_is_pll_div_available(clk_src)) {
					sys_freq = (pll_src_clock * pll_mul_values[j]) / hse_prediv_values[i];
				} else {
					sys_freq = pll_src_clock * pll_mul_values[j];
				}
				if (sys_freq > MAL_HSPEC_STM32F0_MAX_FREQUENCY) {
					continue;
				}
				// Check if it is a better fit
				uint64_t new_freq_diff = abs_int64((int64_t)sys_freq - (int64_t)target_frequency);
				if (new_freq_diff < freq_diff) {
					freq_diff = new_freq_diff;
					if (0 == freq_diff) {
						break;
					}
				}
			}
			if (0 == freq_diff) {
				break;
			}
		}
		// Adjust target frequency
		target_frequency = target_frequency - freq_diff;
		// Set flash latency before switching
		if (target_frequency > 24000000) {
			FLASH_SetLatency(FLASH_Latency_1);
		}
		// Set divider
		if (mal_hspec_stm32f0_is_pll_div_available(clk_src)) {
			RCC_PREDIV1Config(hse_prediv_reg_values[i]);
		}
		// Set PLL multiplier and input
		uint32_t pll_source = MAL_HSPEC_STM32F0_HSI_PLL_SRC;
		if (MAL_SYSTEM_CLK_SRC_EXTERNAL == clk_src) {
			pll_source = RCC_PLLSource_HSE;
		}
		RCC_PLLConfig(pll_source, pll_mul_reg_values[j]);
		// Turn PLL on
		RCC_PLLCmd(ENABLE);
		while (!(RCC->CR & RCC_CR_PLLRDY));
		// Set source
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	}

	if (clk->src != MAL_SYSTEM_CLK_SRC_AUTO && clk->src != clk_src) {
		return MAL_ERROR_CLOCK_ERROR;
	} else if (SystemCoreClock != requested_frequency) {
		return MAL_ERROR_CLOCK_ERROR;
	}
	return MAL_ERROR_OK;
}

// This code is copied from newlib...
void initialise_memory(void) {
	// Copy the DATA segment from Flash to RAM (inlined).
	unsigned int *data_start = &_sdata;
	unsigned int *source_data = &_sidata;
	while (data_start < &_edata) {
		*(data_start++) = *(source_data++);
	}
	// Zero fill the BSS section (inlined).
	data_start = &__bss_start__;
	while (data_start < &__bss_end__) {
		*(data_start++) = 0;
	}
}
