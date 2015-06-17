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
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAL.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>

#include "hspec/mal_hspec.h"
#include "mal_hspec_stm32f0.h"
#include "std/mal_stdint.h"
#include "std/mal_math.h"

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

#define GET_GPIO_PIN(pin) (1 << pin)

#define INCREMENT_TICK(timer) do { \
	if (tick_handles[timer] != NULL) { \
		(*tick_handles[timer]) ++; \
	} \
}while(0)

#define I2C_MIN_AF_DELAY	0.00000005f

extern void mal_clock_set_system_clock(uint64_t clock);

typedef enum {
	I2C_STATE_START,
	I2C_STATE_RECEIVING,
	I2C_STATE_ERROR,
	I2C_STATE_WAITING_TRANSFER_COMPLETE,
	I2C_STATE_TRANSMITTING,
	I2C_STATE_WAIT_STOP
} i2c_states_e;

typedef struct {
	mal_hspec_i2c_msg_s *msg;
	bool is_active;
	volatile i2c_states_e state;
	volatile uint8_t data_ptr;
	volatile mal_hspec_i2c_cmd_e cmd;
	I2C_TypeDef *stm_handle;
} i2c_handle_s;

static void initialise_memory(void);

static GPIO_TypeDef* get_gpio_typedef(mal_hspec_port_e port);

static GPIOSpeed_TypeDef get_gpio_speed(uint64_t speed);

static uint32_t get_rcc_gpio_port(mal_hspec_port_e port);

static mal_error_e init_timer_rcc(mal_hspec_timer_e timer);

static uint32_t get_rcc_timer(mal_hspec_timer_e timer);

static TIM_TypeDef* get_timer_typedef(mal_hspec_timer_e timer);

static mal_error_e get_pin_af(mal_hspec_gpio_s *gpio, mal_hspec_stm32f0_af_e af, uint8_t *function);

static void i2c_start_transfer(i2c_handle_s *i2c_handle);

static void i2c_interrupt_transmit_handler(i2c_handle_s *handle);

static void i2c_interrupt_receive_handler(i2c_handle_s *handle);

static void i2c_common_errors(i2c_handle_s *handle);

static void i2c_common(i2c_handle_s *handle);

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

static volatile uint64_t *tick_handles[MAL_HSPEC_TIMER_SIZE];

static i2c_handle_s i2c_handle_1 = {
	.is_active = false,
	.stm_handle = I2C1
};

static i2c_handle_s i2c_handle_2 = {
	.is_active = false,
	.stm_handle = I2C2
};

static mal_hspec_can_tx_callback_t can_tx_callback = NULL;
static mal_hspec_can_rx_callback_t can_rx_callback = NULL;

mal_error_e mal_hspec_stm32f0_gpio_init(mal_hpsec_gpio_init_s *gpio_init) {
	mal_error_e result;
	// Check gpio
	result = mal_hspec_is_gpio_valid(&gpio_init->gpio);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Enable clock domain
	RCC_AHBPeriphClockCmd(get_rcc_gpio_port(gpio_init->gpio.port), ENABLE);
	// Set GPIO
	GPIO_InitTypeDef init;
	if (MAL_GPIO_DIR_IN == gpio_init->direction) {
		init.GPIO_Mode = GPIO_Mode_IN;
	} else {
		init.GPIO_Mode = GPIO_Mode_OUT;
		if (MAL_GPIO_OUT_PP == gpio_init->output_config) {
			init.GPIO_OType = GPIO_OType_PP;
		} else {
			init.GPIO_OType = GPIO_OType_OD;
		}
	}
	init.GPIO_Pin = GET_GPIO_PIN(gpio_init->gpio.pin);
	switch (gpio_init->pupd) {
	case MAL_GPIO_PUPD_PU:
		init.GPIO_PuPd = GPIO_PuPd_UP;
		break;
	case MAL_GPIO_PUPD_PD:
		init.GPIO_PuPd = GPIO_PuPd_DOWN;
		break;
	case MAL_GPIO_PUPD_NONE:
	default:
		init.GPIO_PuPd = GPIO_PuPd_NOPULL;
		break;
	}
	init.GPIO_Speed = get_gpio_speed(gpio_init->speed);
	GPIO_Init(get_gpio_typedef(gpio_init->gpio.port), &init);

	return MAL_ERROR_OK;
}

void mal_hspec_stm32f0_startup() {
	// Initialises basic clocks and systems
	SystemInit();

	initialise_memory();
	mal_hspec_initialise_system_clk();

	// Update system core clock for stm32f0-stdperiph drivers
	SystemCoreClockUpdate();
}

mal_error_e mal_hspec_stm32f0_set_system_clk(const mal_hspec_system_clk_s *clk) {
	uint32_t timeout_counter;
	uint32_t hserdy_status;
	uint64_t src_clk_freq;
	mal_hspec_system_clk_src_e clk_src = clk->src;
	uint64_t target_frequency = clk->frequency;
	// Choose clk source
	switch (clk->src) {
	case MAL_HSPEC_SYSTEM_CLK_SRC_AUTO:
		// Since external clock is more precise than internal, MAL will favor
		// external in this case.
	case MAL_HSPEC_SYSTEM_CLK_SRC_EXTERNAL:
		// Check if there is an external source
		clk_src = MAL_HSPEC_SYSTEM_CLK_SRC_EXTERNAL;
		src_clk_freq = mal_hspec_get_external_clk_freq();
		if (0 == src_clk_freq) {
			// No external, switch to internal
			src_clk_freq = HSI_VALUE;
			clk_src = MAL_HSPEC_SYSTEM_CLK_SRC_INTERNAL;
		}
		break;
	case MAL_HSPEC_SYSTEM_CLK_SRC_INTERNAL:
	default:
		// Set HSI on
		src_clk_freq = HSI_VALUE;
		break;
	}
	// Start HSE clock if source
	if (MAL_HSPEC_SYSTEM_CLK_SRC_EXTERNAL == clk_src) {
		RCC_HSEConfig(RCC_HSE_ON);
		if (SUCCESS != RCC_WaitForHSEStartUp()) {
			// HSE startup was unsuccessful, use HSI
			clk_src = MAL_HSPEC_SYSTEM_CLK_SRC_INTERNAL;
			src_clk_freq = HSI_VALUE;
		}
	}
	// Set flash prefetch buffer before changing clock
	FLASH_PrefetchBufferCmd(ENABLE);
	// Check if clock can be used without PLL
	if (clk->frequency == src_clk_freq) {
		// Set flash latency before switching
		if (src_clk_freq > 24000000) {
			FLASH_SetLatency(FLASH_Latency_1);
		}
		// Set SW
		if (MAL_HSPEC_SYSTEM_CLK_SRC_INTERNAL == clk_src) {
			RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
		} else {
			RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
		}
	} else {
		// Cap target frequency
		if (clk->frequency > MAL_HSPEC_STM32F0_MAX_FREQUENCY) {
			target_frequency = MAL_HSPEC_STM32F0_MAX_FREQUENCY;
		}
		// Select PLL input clock
		uint64_t pll_src_clock;
		if (clk_src == MAL_HSPEC_SYSTEM_CLK_SRC_INTERNAL) {
			pll_src_clock = HSI_VALUE / 2;
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
				if (clk_src == MAL_HSPEC_SYSTEM_CLK_SRC_EXTERNAL) {
					sys_freq = (pll_src_clock / hse_prediv_values[i]) * pll_mul_values[j];
				} else {
					sys_freq = pll_src_clock * pll_mul_values[j];
				}
				if (sys_freq > MAL_HSPEC_STM32F0_MAX_FREQUENCY) {
					continue;
				}
				// Check if it is a better fit
				uint64_t new_freq_diff = abs_int64(sys_freq - target_frequency);
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
		// Set HSE divider
		if (MAL_HSPEC_SYSTEM_CLK_SRC_EXTERNAL == clk_src) {
			RCC_PREDIV1Config(hse_prediv_reg_values[i]);
		}
		// Set PLL multiplier and input
		uint32_t pll_source = RCC_PLLSource_HSI_Div2;
		if (MAL_HSPEC_SYSTEM_CLK_SRC_EXTERNAL == clk_src) {
			pll_source = RCC_PLLSource_PREDIV1;
		}
		RCC_PLLConfig(pll_source, pll_mul_reg_values[j]);
		// Turn PLL on
		RCC_PLLCmd(ENABLE);
		while (!(RCC->CR & RCC_CR_PLLRDY));
		// Set source
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
	}

	// Set system clock
	mal_clock_set_system_clock(target_frequency);

	if (clk->src != MAL_HSPEC_SYSTEM_CLK_SRC_AUTO && clk->src != clk_src) {
		return MAL_ERROR_CLOCK_ERROR;
	} else if (SystemCoreClock != clk->frequency) {
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

GPIO_TypeDef* get_gpio_typedef(mal_hspec_port_e port) {
	switch (port) {
	case MAL_HSPEC_PORT_A:
		return GPIOA;
	case MAL_HSPEC_PORT_B:
		return GPIOB;
	case MAL_HSPEC_PORT_C:
		return GPIOC;
	case MAL_HSPEC_PORT_D:
		return GPIOD;
	case MAL_HSPEC_PORT_E:
		return GPIOE;
	case MAL_HSPEC_PORT_F:
		return GPIOF;
	default:
		return NULL;
	}
}

GPIOSpeed_TypeDef get_gpio_speed(uint64_t speed) {
	if (speed >= 50000000) {
		return GPIO_Speed_Level_3;
	} else if (speed >= 10000000) {
		return GPIO_Speed_Level_2;
	}
	return GPIO_Speed_Level_1;
}

mal_error_e mal_hspec_stm32f0_set_gpio(mal_hspec_gpio_s *gpio, bool value) {
	BitAction action = Bit_RESET;
	mal_error_e result;
	// Check gpio
	result = mal_hspec_is_gpio_valid(gpio);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	if (value) {
		action = Bit_SET;
	}
	GPIO_WriteBit(get_gpio_typedef(gpio->port), GET_GPIO_PIN(gpio->pin),
			action);

	return MAL_ERROR_OK;
}

uint32_t get_rcc_gpio_port(mal_hspec_port_e port) {
	switch (port) {
	case MAL_HSPEC_PORT_A:
		return RCC_AHBPeriph_GPIOA;
	case MAL_HSPEC_PORT_B:
		return RCC_AHBPeriph_GPIOB;
	case MAL_HSPEC_PORT_C:
		return RCC_AHBPeriph_GPIOC;
	case MAL_HSPEC_PORT_D:
		return RCC_AHBPeriph_GPIOD;
	case MAL_HSPEC_PORT_E:
		return RCC_AHBPeriph_GPIOE;
	case MAL_HSPEC_PORT_F:
	default:
		return RCC_AHBPeriph_GPIOA;
	}
}

bool mal_hspec_stm32f0_get_gpio(mal_hspec_gpio_s *gpio) {
	uint8_t result = GPIO_ReadInputDataBit(get_gpio_typedef(gpio->port),
			GET_GPIO_PIN(gpio->pin));
	if (Bit_SET == result) {
		return true;
	}
	return false;
}

mal_error_e mal_hspec_stm32f0_toggle_gpio(mal_hspec_gpio_s *gpio) {
	uint8_t state = GPIO_ReadOutputDataBit(get_gpio_typedef(gpio->port), GET_GPIO_PIN(gpio->pin));
	if (Bit_SET == state) {
		mal_hspec_stm32f0_set_gpio(gpio, false);
	} else {
		mal_hspec_stm32f0_set_gpio(gpio, true);
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_init_tick(mal_hspec_timer_e timer, float frequency, uint8_t precision,
		volatile uint64_t *tick_handle) {
	mal_error_e result;
	// Initialise peripheral clock
	result = init_timer_rcc(timer);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Get timer
	TIM_TypeDef *tim = get_timer_typedef(timer);
	// Get timer frequency
	uint64_t timer_frequency;
	result = mal_hspec_stm32f0_get_timer_input_clk(timer, &timer_frequency);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Initialise time base timer
	TIM_TimeBaseInitTypeDef params;
	TIM_TimeBaseStructInit(&params);
	params.TIM_CounterMode = TIM_CounterMode_Up;
	// Choose longest count to get maximum precision
	uint64_t prescaler;
	int64_t period;
	for (period = UINT16_MAX; period > 0; period--) {
		float value = ((float)timer_frequency) / (frequency * ((float)period));
		// Check precision
		int remainder = (value - (uint64_t)value) * 100;
		if (remainder >= precision) {
			prescaler = (int)ceilf(value);
		} else if (remainder <= (100 - precision)) {
			prescaler = (int)value;
		} else {
			continue;
		}
		// Check that prescaler is not 0. This would happen if prescaler in
		// float would be 0.1 or lower.
		if (prescaler > 0) {
			break;
		}
	}
	params.TIM_Prescaler = prescaler - 1;
	params.TIM_Period = period;
	TIM_TimeBaseInit(tim, &params);
	// Save tick handle
	tick_handles[timer] = tick_handle;
	// Enable NVIC interrupt for timer
	IRQn_Type irq = mal_hspec_stm32f0_get_timer_update_irq(timer);
	NVIC_EnableIRQ(irq);
	NVIC_SetPriority(irq, 2); // Find a way to manage priorities for interrupts
	// Enable timer interrupt
	TIM_ITConfig(tim, TIM_IT_Update, ENABLE);
	// Enable timer
	TIM_Cmd(tim, ENABLE);

	return MAL_ERROR_OK;
}

mal_error_e init_timer_rcc(mal_hspec_timer_e timer) {
switch (timer) {
case MAL_HSPEC_TIMER_1:
case MAL_HSPEC_TIMER_15:
case MAL_HSPEC_TIMER_16:
case MAL_HSPEC_TIMER_17:
	RCC_APB2PeriphClockCmd(get_rcc_timer(timer), ENABLE);
	break;
case MAL_HSPEC_TIMER_2:
case MAL_HSPEC_TIMER_3:
case MAL_HSPEC_TIMER_6:
case MAL_HSPEC_TIMER_7:
case MAL_HSPEC_TIMER_14:
default:
	RCC_APB1PeriphClockCmd(get_rcc_timer(timer), ENABLE);
	break;
}
return MAL_ERROR_OK;
}

uint32_t get_rcc_timer(mal_hspec_timer_e timer) {
switch (timer) {
case MAL_HSPEC_TIMER_1:
	return RCC_APB2Periph_TIM1;
case MAL_HSPEC_TIMER_2:
	return RCC_APB1Periph_TIM2;
case MAL_HSPEC_TIMER_3:
	return RCC_APB1Periph_TIM3;
case MAL_HSPEC_TIMER_6:
	return RCC_APB1Periph_TIM6;
case MAL_HSPEC_TIMER_7:
	return RCC_APB1Periph_TIM7;
case MAL_HSPEC_TIMER_14:
	return RCC_APB1Periph_TIM14;
case MAL_HSPEC_TIMER_15:
	return RCC_APB2Periph_TIM15;
case MAL_HSPEC_TIMER_16:
	return RCC_APB2Periph_TIM16;
case MAL_HSPEC_TIMER_17:
default:
	return RCC_APB2Periph_TIM17;
}
}

TIM_TypeDef* get_timer_typedef(mal_hspec_timer_e timer) {
	switch (timer) {
	case MAL_HSPEC_TIMER_1:
		return TIM1;
	case MAL_HSPEC_TIMER_2:
		return TIM2;
	case MAL_HSPEC_TIMER_3:
		return TIM3;
	case MAL_HSPEC_TIMER_6:
		return TIM6;
	case MAL_HSPEC_TIMER_7:
		return TIM7;
	case MAL_HSPEC_TIMER_14:
		return TIM14;
	case MAL_HSPEC_TIMER_15:
		return TIM15;
	case MAL_HSPEC_TIMER_16:
		return TIM16;
	case MAL_HSPEC_TIMER_17:
		return TIM17;
	default:
		return NULL;
	}
}

mal_error_e mal_hspec_stm32f0_get_timer_input_clk(mal_hspec_timer_e timer,
	uint64_t *clock) {
	// MCU clocks
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	// APB Prescaler
	uint32_t apb_pre = RCC->CFGR & RCC_CFGR_PPRE;

	*clock = clocks.PCLK_Frequency;
	if (apb_pre != RCC_CFGR_PPRE_DIV1) {
		// Times 2, see clock tree in datasheet
		*clock += *clock;
	}
	return MAL_ERROR_OK;
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void) {
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	// Handles tick
	INCREMENT_TICK(MAL_HSPEC_TIMER_1);
}

void TIM2_IRQHandler(void) {
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	// Handles tick
	INCREMENT_TICK(MAL_HSPEC_TIMER_2);
}

void TIM3_IRQHandler(void) {
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	// Handles tick
	INCREMENT_TICK(MAL_HSPEC_TIMER_3);
}

void TIM6_DAC_IRQHandler(void) {
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	// Handles tick
	INCREMENT_TICK(MAL_HSPEC_TIMER_6);
}

void TIM7_IRQHandler(void) {
	TIM_ClearFlag(TIM7, TIM_FLAG_Update);
	// Handles tick
	INCREMENT_TICK(MAL_HSPEC_TIMER_7);
}

void TIM14_IRQHandler(void) {
	TIM_ClearFlag(TIM14, TIM_FLAG_Update);
	// Handles tick
	INCREMENT_TICK(MAL_HSPEC_TIMER_14);
}

void TIM15_IRQHandler(void) {
	TIM_ClearFlag(TIM15, TIM_FLAG_Update);
	// Handles tick
	INCREMENT_TICK(MAL_HSPEC_TIMER_15);
}

void TIM16_IRQHandler(void) {
	TIM_ClearFlag(TIM16, TIM_FLAG_Update);
	// Handles tick
	INCREMENT_TICK(MAL_HSPEC_TIMER_16);
}

void TIM17_IRQHandler(void) {
	TIM_ClearFlag(TIM17, TIM_FLAG_Update);
	// Handles tick
	INCREMENT_TICK(MAL_HSPEC_TIMER_17);
}

mal_error_e mal_hspec_stm32f0_i2c_master_init(mal_hspec_i2c_init_s *init) {
	uint64_t i2c_clock;
	// Set I2C clock source
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	// See STM32F0 user manual for I2C clock requirements.
	uint64_t min_clk = ((float)(8 * init->bitrate)) / (1.0f - 0.00000052f * ((float)init->bitrate));
	if (MAL_HSPEC_I2C_1 == init->interface) {
		if (SystemCoreClock > min_clk) {
			RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
			i2c_clock = SystemCoreClock;
		} else if (HSI_VALUE > min_clk) {
			RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);
			i2c_clock = HSI_VALUE;
		} else {
			return MAL_ERROR_CLOCK_ERROR;
		}
	} else if (MAL_HSPEC_I2C_2 == init->interface) {
		i2c_clock = clocks.PCLK_Frequency;
		if (clocks.PCLK_Frequency < min_clk) {
			return MAL_ERROR_CLOCK_ERROR;
		}
	} else {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	if (init->bitrate >= ((4 * clocks.PCLK_Frequency) / 3)) {
		return MAL_ERROR_CLOCK_ERROR;
	}
	// Enable GPIO clocks
	RCC_AHBPeriphClockCmd(get_rcc_gpio_port(init->scl_gpio->port), ENABLE);
	RCC_AHBPeriphClockCmd(get_rcc_gpio_port(init->sda_gpio->port), ENABLE);
	// Enable I2C clock
	if (MAL_HSPEC_I2C_1 == init->interface) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	} else {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	}
	// Select correct I2C pin functions
	mal_hspec_stm32f0_af_e scl_af;
	mal_hspec_stm32f0_af_e sda_af;
	if (MAL_HSPEC_I2C_1 == init->interface) {
		scl_af = MAL_HSPEC_STM32F0_AF_I2C1_SCL;
		sda_af = MAL_HSPEC_STM32F0_AF_I2C1_SDA;
	} else {
		scl_af = MAL_HSPEC_STM32F0_AF_I2C2_SCL;
		sda_af = MAL_HSPEC_STM32F0_AF_I2C2_SDA;
	}
	// Configure alternate function
	mal_error_e result;
	uint8_t function;
	result = get_pin_af(init->scl_gpio, scl_af, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(get_gpio_typedef(init->scl_gpio->port), init->scl_gpio->pin, function);

	result = get_pin_af(init->sda_gpio, sda_af, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(get_gpio_typedef(init->sda_gpio->port), init->sda_gpio->pin, function);
	// Configure GPIOs
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = GET_GPIO_PIN(init->scl_gpio->pin);
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_OType = GPIO_OType_OD;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(get_gpio_typedef(init->scl_gpio->port), &gpio_init);

	gpio_init.GPIO_Pin = GET_GPIO_PIN(init->sda_gpio->pin);
	GPIO_Init(get_gpio_typedef(init->sda_gpio->port), &gpio_init);
	// Compute timing
	// Determine fall time
	float tf = 0.0000003; // 300 ns
	if (init->bitrate > 400000) {
		// 12 ns if bitrate is higher than 400k
		tf = 0.00000012;
	}
	// Determine rise time
	float tr = 0.000001; // 1us
	if (init->bitrate > 100000 && init->bitrate <= 400000) {
		// If speed is fast mode (between 100k and 400k), 300ns
		tr = 0.0000003;
	} else if (init->bitrate > 400000) {
		// 120ns if faster.
		tr = 0.00000012;
	}
	// Determine minimum setup time
	float tsu = 0.00000015; // 250ns
	if (init->bitrate > 100000 && init->bitrate <= 400000) {
		// If speed is fast mode (between 100k and 400k), 100ns
		tsu = 0.0000001;
	} else if (init->bitrate > 400000) {
		// 50ns if faster.
		tsu = 0.00000005;
	}
	// Determine valid data time
	float tvd = 0.00000345; // 3.45us
	if (init->bitrate > 100000 && init->bitrate <= 400000) {
		// If speed is fast mode (between 100k and 400k), 900ns
		tsu = 0.0000009;
	} else if (init->bitrate > 400000) {
		// 450ns if faster.
		tsu = 0.00000045;
	}
	// Determine true half period. Bitrate = Fall Time + Time Low + Rise Time + Time High
	float half_period = ((1.0f / ((float)init->bitrate)) - tf - tr) / 2;
	// Try prescaller values to find a match to all values
	uint32_t presc;
	uint32_t scldel;
	uint32_t sdadel;
	uint32_t scllh;
	for (presc = 0; presc <= 15; presc++) {
		// Compute prescaled period
		float ti2c = (1.0f / ((float)i2c_clock));
		float tpresc = ti2c * (presc + 1);
		// Compute SCL delay for setup time
		scldel = ((tr + tsu) / tpresc) - 1;
		if (scldel > 15) {
			// SCLDEL is only 4 bits long, must use another prescaler value
			continue;
		}
		// Compute SDA delay for hold time. The formula uses minimum analog
		// filter delay which is 50ns.
		sdadel = (tf - I2C_MIN_AF_DELAY - 3 * ti2c) / tpresc;
		if (sdadel > 15) {
			// SDADEL is only 4 bits long, must use another prescaler value
			continue;
		}
		// Compute SCL low and high time
		scllh = ((half_period - I2C_MIN_AF_DELAY - 2 * ti2c) / tpresc) - 1;
		if (scllh > 255) {
			// SCLH and SCLL are 8 bits
			continue;
		}
		// All settings are good, end loop
		break;
	}
	// Get I2C typedef
	I2C_TypeDef *i2c_typedef;
	if (MAL_HSPEC_I2C_1 == init->interface) {
		i2c_typedef = I2C1;
	} else {
		i2c_typedef = I2C2;
	}
	// Configure I2C
	I2C_InitTypeDef i2c_init;
	i2c_init.I2C_Mode = I2C_Mode_I2C;
	i2c_init.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	i2c_init.I2C_DigitalFilter = 0;
	i2c_init.I2C_OwnAddress1 = 0;
	i2c_init.I2C_Ack = I2C_Ack_Enable;
	i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c_init.I2C_Timing = (presc << 28) | (scldel << 20) | (sdadel << 16) | (scllh << 8) | scllh;
	I2C_Init(i2c_typedef, &i2c_init);
	// Enable interrupt
	if (MAL_HSPEC_I2C_1 == init->interface) {
		NVIC_EnableIRQ(I2C1_IRQn);
	} else {
		NVIC_EnableIRQ(I2C2_IRQn);
	}
	I2C_ITConfig(i2c_typedef, I2C_IT_RXI|I2C_IT_TXI|I2C_IT_TCI|I2C_IT_NACKI|I2C_IT_ERRI|I2C_IT_STOPI, ENABLE);
	// Enable I2C, finally!
	I2C_Cmd(i2c_typedef, ENABLE);

	return MAL_ERROR_OK;
}

mal_error_e get_pin_af(mal_hspec_gpio_s *gpio, mal_hspec_stm32f0_af_e af, uint8_t *function) {
	const mal_hspec_stm32f0_af_e (*afs)[MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE][MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE];
	mal_error_e result;
	int port_af, pin_af;

	result = mal_hspec_stm32f0_get_port_afs(gpio->port, &afs);
	if (MAL_ERROR_OK != result) {
		return result;
	}

	for (port_af = 0; port_af < MAL_HSPEC_STM32F0_GPIO_PORT_AF_SIZE; port_af++) {
		for (pin_af = 0; pin_af < MAL_HSPEC_STM32F0_GPIO_PIN_AF_SIZE; pin_af++) {
			if ((*afs)[gpio->pin][port_af][pin_af] == af) {
				*function = port_af;
				return MAL_ERROR_OK;
			}
		}
	}

	return MAL_ERROR_HARDWARE_INVALID;
}

mal_error_e mal_hspec_stm32f0_i2c_transfer(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg) {
	i2c_handle_s *i2c_handle;
	// Fetch I2C handle
	if (MAL_HSPEC_I2C_1 == interface) {
		i2c_handle = &i2c_handle_1;
	} else if (MAL_HSPEC_I2C_2 == interface) {
		i2c_handle = &i2c_handle_2;
	} else {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	// Make sure I2C is free
	if (i2c_handle->is_active) {
		return MAL_ERROR_HARDWARE_UNAVAILABLE;
	}
	// Save msg
	i2c_handle->msg = msg;

	i2c_start_transfer(i2c_handle);

	return MAL_ERROR_OK;
}

void i2c_start_transfer(i2c_handle_s *i2c_handle) {
	i2c_handle->cmd = i2c_handle->msg->packet.cmd;
	// Start transfer
	uint32_t transfer_type = I2C_Generate_Start_Read;
	if (MAL_HSPEC_I2C_WRITE == i2c_handle->cmd) {
		transfer_type = I2C_Generate_Start_Write;
	}
	I2C_TransferHandling(i2c_handle->stm_handle, i2c_handle->msg->packet.address, i2c_handle->msg->packet.packet_size, I2C_SoftEnd_Mode, transfer_type);
}

void I2C1_IRQHandler(void) {
	if (MAL_HSPEC_I2C_WRITE == i2c_handle_1.cmd) {
		i2c_interrupt_transmit_handler(&i2c_handle_1);
	} else {

	}
}

void I2C2_IRQHandler(void) {
	if (MAL_HSPEC_I2C_WRITE == i2c_handle_2.cmd) {
		i2c_interrupt_transmit_handler(&i2c_handle_2);
	} else {

	}
}

void i2c_interrupt_transmit_handler(i2c_handle_s *handle) {
	// Common errors
	i2c_common_errors(handle);
	// Nack event
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_NACKF) == SET) {
		// Clear interrupt
		I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_NACKF);
		if (handle->state == I2C_STATE_TRANSMITTING) {
			mal_hspec_i2c_result_e result;
			// Check if write is complete
			if (handle->data_ptr >= handle->msg->packet.packet_size) {
				// Change state
				result = MAL_HSPEC_I2C_NACK_COMPLETE;
			} else {
				// Transfer is incomplete
				result = MAL_HSPEC_I2C_NACK_INCOMPLETE;
			}
			handle->msg->callback(&handle->msg->packet, result, &handle->msg);
			// Next state
			handle->state = I2C_STATE_WAIT_STOP;
		} else {
			// Unknown sate for nack
			handle->msg->callback(&handle->msg->packet, MAL_HSPEC_I2C_NACK_INCOMPLETE, &handle->msg);
			// Next state
			handle->state = I2C_STATE_WAIT_STOP;
		}
	}
	// Transmitter status
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_TXIS) == SET) {
		// Make sure we are receiving
		if (I2C_STATE_TRANSMITTING != handle->state) {
			// Stop receiving
			I2C_GenerateSTOP(handle->stm_handle, ENABLE);
			// We are in error
			handle->state = I2C_STATE_ERROR;
			// Clear interrupt
			I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_TXIS);
		} else {
			// Check if there still data to be sent
			if (handle->data_ptr >= handle->msg->packet.packet_size) {
				// Clear interrupt
				I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_TXIS);
				// Flag error
				handle->state = I2C_STATE_WAITING_TRANSFER_COMPLETE;
			} else {
				// Transmit next data
				I2C_SendData(handle->stm_handle, handle->msg->packet.buffer[handle->data_ptr++]);
			}
		}
	}
	// Common states
	i2c_common(handle);
}

void i2c_interrupt_receive_handler(i2c_handle_s *handle) {
	// Common errors
	i2c_common_errors(handle);
	// Receiver status
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_RXNE) == SET) {
		// Make sure we are receiving
		if (I2C_STATE_RECEIVING != handle->state) {
			// Clear interrupt
			I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_RXNE);
			// Stop receiving
			I2C_GenerateSTOP(handle->stm_handle, ENABLE);
			// We are in error
			handle->state = I2C_STATE_ERROR;
		} else {
			// Read data, also clears interrupt
			handle->msg->packet.buffer[handle->data_ptr++] = I2C_ReceiveData(handle->stm_handle);
			// Check if data is complete
			if (handle->data_ptr >= handle->msg->packet.packet_size) {
				// Change state
				handle->state = I2C_STATE_WAITING_TRANSFER_COMPLETE;
			}
		}
	}
	// Common states
	i2c_common(handle);
}

void i2c_common_errors(i2c_handle_s *handle) {
	// Bus error
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_BERR) == SET) {
		// Clear interrupt
		I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_BERR);
		// Stop bus
		I2C_GenerateSTOP(handle->stm_handle, ENABLE);
		// Start or stop out of place switch to error state
		handle->state = I2C_STATE_ERROR;
	}
	// Arbitration loss error
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_ARLO) == SET) {
		// Clear interrupt
		I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_ARLO);
		// Start or stop out of place switch to error state
		handle->state = I2C_STATE_ERROR;
	}
}

void i2c_common(i2c_handle_s *handle) {
	// Check if transmission is complete
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_TCR) == SET) {
		// Transmit complete, stop transfer, clear interrupt with stop
		I2C_GenerateSTOP(handle->stm_handle, ENABLE);
		// Make sure it was expected
		if (I2C_STATE_WAITING_TRANSFER_COMPLETE != handle->state) {
			// We are in error
			handle->state = I2C_STATE_ERROR;
		} else {
			handle->msg->callback(&handle->msg->packet, MAL_HSPEC_I2C_SUCCESS, &handle->msg);
			// Next state
			handle->state = I2C_STATE_WAIT_STOP;
		}
	}
	// Check for stop
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_STOPF) == SET) {
		// Clear interrupt
		I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_STOPF);
		// Check if stop is expected
		if (I2C_STATE_WAIT_STOP != handle->state) {
			handle->msg->callback(&handle->msg->packet, MAL_HSPEC_I2C_BUS_ERROR, &handle->msg);
		}
		// Check if a new message can be started
		if (handle->msg != NULL) {
			i2c_start_transfer(handle);
		} else {
			handle->is_active = false;
		}
	}
}

mal_error_e mal_hspec_stm32f0_can_init(mal_hspec_can_init_s *init) {
	// Enable GPIO clocks
	RCC_AHBPeriphClockCmd(get_rcc_gpio_port(init->tx_gpio->port), ENABLE);
	RCC_AHBPeriphClockCmd(get_rcc_gpio_port(init->rx_gpio->port), ENABLE);
	// Configure alternate function
	mal_error_e result;
	uint8_t function;
	result = get_pin_af(init->tx_gpio, MAL_HSPEC_STM32F0_AF_CAN_TX, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(get_gpio_typedef(init->tx_gpio->port), init->tx_gpio->pin, function);

	result = get_pin_af(init->rx_gpio, MAL_HSPEC_STM32F0_AF_CAN_RX, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(get_gpio_typedef(init->rx_gpio->port), init->rx_gpio->pin, function);
	// Configure GPIOs
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = GET_GPIO_PIN(init->tx_gpio->pin);
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(get_gpio_typedef(init->tx_gpio->port), &gpio_init);

	gpio_init.GPIO_Pin = GET_GPIO_PIN(init->rx_gpio->pin);
	GPIO_Init(get_gpio_typedef(init->rx_gpio->port), &gpio_init);
	// Enable CAN clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);
	// Clear CAN
	CAN_DeInit(CAN);
	// Get APB Clock
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	// Compute bit time
	uint32_t prescaler;
	uint32_t tseg1;
	int32_t tseg2;
	uint32_t sjw;
	bool done = false;
	for (prescaler = 1; prescaler <= 1024; prescaler++) {
		uint32_t tq_total = (clocks.PCLK_Frequency) / (prescaler * init->bitrate);
		// TSEG1 must be at least 2 time quantas long because of it includes
		// the propagation segment which takes 1 time quanta.
		for (tseg1 = 2; tseg1 < 16; tseg1++) {
			tseg2 = tq_total - tseg1;
			// Here are the rules to pass this point.
			// 1. TSEG2 must be equal or less to TSEG1. This is to ensure that
			//    the sample point is not before the 50% mark.
			if (tseg2 > tseg1) {
				continue;
			}
			// 2. Since TSEG2 must have a time, it cannot be 0.
			if (tseg2 <= 0) {
				continue;
			}
			// 3. TSEG2 has a maximum of 8 time quantas.
			if (tseg2 > 8) {
				continue;
			}
			// We're, now we have to find a suitable synchronisation jump
			// width.
			for (sjw = 4; sjw >= 1; sjw--) {
				// Jump must not be longer than TSEG2 because the jump lengthen
				// or shorten TSEG2.
				if (sjw < tseg2) {
					done = true;
					break;
				}
			}
			if (done) {
				break;
			}
		}
		if (done) {
			break;
		}
	}
	if (!done) {
		return MAL_ERROR_CLOCK_ERROR;
	}
	// Save call backs
	can_tx_callback = init->tx_callback;
	can_rx_callback = init->rx_callback;
	// Configure CAN
	CAN_InitTypeDef can_init;
	CAN_StructInit(&can_init);
	can_init.CAN_BS1 = tseg1 - 1;
	can_init.CAN_BS2 = tseg2 - 1;
	can_init.CAN_SJW = sjw - 1;
	can_init.CAN_Prescaler = prescaler;
	if (CAN_InitStatus_Success != CAN_Init(CAN, &can_init)) {
		return MAL_ERROR_INIT_FAILED;
	}
	// Enable interrupts
	// 30 equates to CAN_IRQ. However, the name of the constant changes based
	// on the MCU because it is not available on all of them. It is simpler to
	// use the constant directly. If the MCU does not support CAN, the code
	// will not get here.
	NVIC_EnableIRQ(30);
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_FMP1, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_TME, ENABLE);
}

void CEC_CAN_IRQHandler(void) {
	// Check if transmit is empty
	if (CAN_GetITStatus(CAN, CAN_IT_TME) == SET) {
		if (can_tx_callback != NULL) {
			mal_hspec_can_msg_s *msg;
			can_tx_callback(&msg);
			if (msg != NULL) {
				CanTxMsg can_msg;
				if (MAL_HSPEC_CAN_ID_STANDARD == msg->id_type) {
					can_msg.StdId = msg->id;
					can_msg.IDE = CAN_Id_Standard;
				} else {
					can_msg.ExtId = msg->id;
					can_msg.IDE = CAN_Id_Extended;
				}
				for (can_msg.DLC = 0; can_msg.DLC < msg->size; can_msg.DLC++) {
					can_msg.Data[can_msg.DLC] = msg->data[can_msg.DLC];
				}
				can_msg.DLC = msg->size;
				can_msg.RTR = CAN_RTR_Data;
				CAN_Transmit(CAN, &can_msg);
			} else {
				CAN_ITConfig(CAN, CAN_IT_TME, DISABLE);
				CAN_ClearITPendingBit(CAN, CAN_IT_TME);
			}
		}
	}
	// Check FIFOs
	while (CAN_MessagePending(CAN, CAN_FIFO0)) {
		CanRxMsg can_msg;
		CAN_Receive(CAN, CAN_FIFO0, &can_msg);
	}
}
