/*
 * Copyright (c) 2019 Olivier Allaire
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

#include "mal_hspec_stm32f0_timer.h"
#include "std/mal_stdlib.h"
#include "std/mal_math.h"
#include "stm32f0/stm32f0xx_rcc.h"
#include "mal_hspec_stm32f0_cmn.h"

static mal_error_e mal_hspec_stm32f0_timer_init_timer_rcc(mal_timer_e timer);

static uint32_t mal_hspec_stm32f0_timer_get_rcc_timer(mal_timer_e timer);

static void mal_hspec_stm32f0_timer_input_capture_interrupt(mal_timer_s *handle, uint16_t flags);

static mal_error_e mal_hspec_stm32f0_timer_get_update_time_base(TIM_TimeBaseInitTypeDef *time_base,
                                                                mal_hertz_t frequency,
                                                                mal_hertz_t delta);

static mal_error_e mal_hspec_stm32f0_timer_common_init(mal_timer_e timer,
                                                       TIM_TimeBaseInitTypeDef *init_time_base,
                                                       mal_timer_s *handle);

static mal_timer_s *timer1;
static mal_timer_s *timer2;
static mal_timer_s *timer3;
static mal_timer_s *timer6;
static mal_timer_s *timer7;
static mal_timer_s *timer14;
static mal_timer_s *timer15;
static mal_timer_s *timer16;
static mal_timer_s *timer17;

static mal_error_e mal_hspec_stm32f0_timer_common_init(mal_timer_e timer,
                                                       TIM_TimeBaseInitTypeDef *init_time_base,
                                                       mal_timer_s *handle) {
    mal_error_e result;
    // Get timer frequency
    uint64_t timer_frequency;
    result = mal_hspec_stm32f0_timer_get_input_clk(&timer_frequency);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Initialise peripheral clock
    result = mal_hspec_stm32f0_timer_init_timer_rcc(timer);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Initialize handle
    handle->timer = timer;
    handle->stm_handle = mal_hspec_stm32f0_timer_get_typedef(timer);
    handle->count_frequency = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ((timer_frequency * 1000ULL) / (uint64_t)init_time_base->TIM_Prescaler);
    // Initialise base timer
    TIM_TimeBaseInit(handle->stm_handle, init_time_base);
    // Save handle
    switch (handle->timer) {
        case MAL_TIMER_1:
            timer1 = handle;
            break;
        case MAL_TIMER_2:
            timer2 = handle;
            break;
        case MAL_TIMER_3:
            timer3 = handle;
            break;
        case MAL_TIMER_6:
            timer6 = handle;
            break;
        case MAL_TIMER_7:
            timer7 = handle;
            break;
        case MAL_TIMER_14:
            timer14 = handle;
            break;
        case MAL_TIMER_15:
            timer15 = handle;
            break;
        case MAL_TIMER_16:
            timer16 = handle;
            break;
        case MAL_TIMER_17:
            timer17 = handle;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }

    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f0_timer_common_task_init(mal_timer_init_task_s *init,
                                                            TIM_TimeBaseInitTypeDef *init_time_base,
                                                            mal_timer_s *handle) {
    mal_error_e result;
    // Initialise common update parts
    result = mal_hspec_stm32f0_timer_common_init(init->timer, init_time_base, handle);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Initialize handle
    handle->callback = init->callback;
    handle->handle = init->callback_handle;
    handle->irq = mal_hspec_stm32f0_get_timer_update_irq(init->timer);
    // Enable NVIC interrupt for timer
    NVIC_EnableIRQ(handle->irq);
    NVIC_SetPriority(handle->irq, 2); // Find a way to manage priorities for interrupts
    // Enable timer interrupt
    TIM_ITConfig(handle->stm_handle, TIM_IT_Update, ENABLE);
    // Enable timer
    TIM_Cmd(handle->stm_handle, ENABLE);
    return MAL_ERROR_OK;
}

mal_error_e mal_timer_direct_init_task(mal_timer_init_task_s *init, const void *direct_init, mal_timer_s *handle) {
	// Read direct init
	mal_hspec_stm32f0_timer_direct_init_s *stm_direct_init;
	stm_direct_init = (mal_hspec_stm32f0_timer_direct_init_s*)direct_init;
	// Initialise time base timer
	TIM_TimeBaseInitTypeDef params;
	TIM_TimeBaseStructInit(&params);
	params.TIM_CounterMode = TIM_CounterMode_Up;
	params.TIM_Prescaler = stm_direct_init->prescaler;
	params.TIM_Period = stm_direct_init->period;
	return mal_hspec_stm32f0_timer_common_task_init(init, &params, handle);
}

static mal_error_e mal_hspec_stm32f0_timer_get_update_time_base(TIM_TimeBaseInitTypeDef *time_base,
                                                                mal_hertz_t frequency,
                                                                mal_hertz_t delta) {
    mal_error_e result;
    // Get timer frequency
    uint64_t timer_frequency;
    result = mal_hspec_stm32f0_timer_get_input_clk(&timer_frequency);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Set parameters
    TIM_TimeBaseStructInit(time_base);
    time_base->TIM_CounterMode = TIM_CounterMode_Up;
    // Choose longest count to get maximum precision
    uint64_t target_frequency = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(frequency);
    uint64_t target_delta = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(delta);
    uint16_t prescaler = 1;
    uint16_t period;
    bool found = false;
    uint64_t smallest_delta = UINT64_MAX;
    for (period = UINT16_MAX; period > 0; period--) {
        for (prescaler = 1; prescaler <= UINT16_MAX; prescaler++) {
            // Resolution is in mHz to handle fractions of Hz
            uint64_t potential_frequency = (timer_frequency * 1000) / ((uint64_t)period * (uint64_t)prescaler);
            uint64_t actual_delta;
            if (potential_frequency >= target_frequency) {
                actual_delta = potential_frequency - target_frequency;
            } else {
                actual_delta = target_frequency - potential_frequency;
            }
            if (actual_delta <= target_delta) {
                found = true;
                break;
            } else {
                if (actual_delta < smallest_delta) {
                    smallest_delta = actual_delta;
                }
            }
            // If we get here and the potential frequency is smaller than the
            // target frequency, we can change the period as the potential
            // frequency is just gonna keep getting smaller if we continue
            if (potential_frequency < target_frequency) {
                break;
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    time_base->TIM_Prescaler = (uint16_t)(prescaler - 1);
    time_base->TIM_Period = period;

    return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_task(mal_timer_init_task_s *init, mal_timer_s *handle) {
	mal_error_e result;
	// Get timer frequency
    TIM_TimeBaseInitTypeDef params;
	result = mal_hspec_stm32f0_timer_get_update_time_base(&params, init->frequency, init->delta);
	if (MAL_ERROR_OK != result) {
        return result;
	}
	return mal_hspec_stm32f0_timer_common_task_init(init, &params, handle);
}

static mal_error_e mal_hspec_stm32f0_timer_init_timer_rcc(mal_timer_e timer) {
switch (timer) {
case MAL_TIMER_1:
case MAL_TIMER_15:
case MAL_TIMER_16:
case MAL_TIMER_17:
	RCC_APB2PeriphClockCmd(mal_hspec_stm32f0_timer_get_rcc_timer(timer), ENABLE);
	break;
case MAL_TIMER_2:
case MAL_TIMER_3:
case MAL_TIMER_6:
case MAL_TIMER_7:
case MAL_TIMER_14:
default:
	RCC_APB1PeriphClockCmd(mal_hspec_stm32f0_timer_get_rcc_timer(timer), ENABLE);
	break;
}
return MAL_ERROR_OK;
}

static uint32_t mal_hspec_stm32f0_timer_get_rcc_timer(mal_timer_e timer) {
    switch (timer) {
        case MAL_TIMER_1:
            return RCC_APB2Periph_TIM1;
        case MAL_TIMER_2:
            return RCC_APB1Periph_TIM2;
        case MAL_TIMER_3:
            return RCC_APB1Periph_TIM3;
        case MAL_TIMER_6:
            return RCC_APB1Periph_TIM6;
        case MAL_TIMER_7:
            return RCC_APB1Periph_TIM7;
        case MAL_TIMER_14:
            return RCC_APB1Periph_TIM14;
        case MAL_TIMER_15:
            return RCC_APB2Periph_TIM15;
        case MAL_TIMER_16:
            return RCC_APB2Periph_TIM16;
        case MAL_TIMER_17:
        default:
            return RCC_APB2Periph_TIM17;
    }
}

TIM_TypeDef* mal_hspec_stm32f0_timer_get_typedef(mal_timer_e timer) {
	switch (timer) {
        case MAL_TIMER_1:
            return TIM1;
        case MAL_TIMER_2:
            return TIM2;
        case MAL_TIMER_3:
            return TIM3;
        case MAL_TIMER_6:
            return TIM6;
        case MAL_TIMER_7:
            return TIM7;
        case MAL_TIMER_14:
            return TIM14;
        case MAL_TIMER_15:
            return TIM15;
        case MAL_TIMER_16:
            return TIM16;
        case MAL_TIMER_17:
            return TIM17;
        default:
            return NULL;
	}
}

mal_error_e mal_hspec_stm32f0_timer_get_input_clk(uint64_t *clock) {
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


void TIM1_CC_IRQHandler(void) {
	// Check which this is a capture interrupt
	uint16_t cc_flags = 0;
	if (TIM1->DIER & TIM_DIER_CC1IE) {
		cc_flags |= TIM1->SR & TIM_FLAG_CC1;
	}
	if (TIM1->DIER & TIM_DIER_CC2IE) {
		cc_flags |= TIM1->SR & TIM_FLAG_CC2;
	}
	if (TIM1->DIER & TIM_DIER_CC3IE) {
		cc_flags |= TIM1->SR & TIM_FLAG_CC3;
	}
	if (TIM1->DIER & TIM_DIER_CC4IE) {
		cc_flags |= TIM1->SR & TIM_FLAG_CC4;
	}
	// Clear interrupts
	TIM_ClearFlag(TIM1, TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_CC3|TIM_FLAG_CC4);
	// Handles interrupt
	if (cc_flags) {
        mal_hspec_stm32f0_timer_input_capture_interrupt(timer1, cc_flags);
	}
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void) {
	// Clear interrupts
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	// Handles interrupt
	if (NULL != timer1 && NULL != timer1->callback) {
        timer1->callback(timer1->handle);
	}
}

void TIM2_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = 0;
	if (TIM2->DIER & TIM_DIER_CC1IE) {
		cc_flags |= TIM2->SR & TIM_FLAG_CC1;
	}
	if (TIM2->DIER & TIM_DIER_CC2IE) {
		cc_flags |= TIM2->SR & TIM_FLAG_CC2;
	}
	if (TIM2->DIER & TIM_DIER_CC3IE) {
		cc_flags |= TIM2->SR & TIM_FLAG_CC3;
	}
	if (TIM2->DIER & TIM_DIER_CC4IE) {
		cc_flags |= TIM2->SR & TIM_FLAG_CC4;
	}
	// Clear interrupts
	TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_CC3|TIM_FLAG_CC4);
	// Handles interrupt
	if (cc_flags) {
        mal_hspec_stm32f0_timer_input_capture_interrupt(timer2, cc_flags);
	} else {
        if (NULL != timer2  && NULL != timer2->callback) {
            timer2->callback(timer2->handle);
        }
	}
}

void TIM3_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = 0;
	if (TIM3->DIER & TIM_DIER_CC1IE) {
		cc_flags |= TIM3->SR & TIM_FLAG_CC1;
	}
	if (TIM3->DIER & TIM_DIER_CC2IE) {
		cc_flags |= TIM3->SR & TIM_FLAG_CC2;
	}
	if (TIM3->DIER & TIM_DIER_CC3IE) {
		cc_flags |= TIM3->SR & TIM_FLAG_CC3;
	}
	if (TIM3->DIER & TIM_DIER_CC4IE) {
		cc_flags |= TIM3->SR & TIM_FLAG_CC4;
	}
	// Clear interrupts
	TIM_ClearFlag(TIM3, TIM_FLAG_Update|TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_CC3|TIM_FLAG_CC4);
	// Handles interrupt
	if (cc_flags) {
        mal_hspec_stm32f0_timer_input_capture_interrupt(timer3, cc_flags);
	} else {
        if (NULL != timer3 && NULL != timer3->callback) {
            timer3->callback(timer3->handle);
        }
	}
}

void TIM6_DAC_IRQHandler(void) {
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	// Handles tick
    if (NULL != timer6 && NULL != timer6->callback) {
        timer6->callback(timer6->handle);
    }
}

void TIM7_IRQHandler(void) {
	TIM_ClearFlag(TIM7, TIM_FLAG_Update);
	// Handles tick
    if (NULL != timer7  && NULL != timer7->callback) {
        timer7->callback(timer7->handle);
    }
}

void TIM14_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = 0;
	if (TIM14->DIER & TIM_DIER_CC1IE) {
		cc_flags = TIM14->SR & TIM_FLAG_CC1;
	}
	// Clear interrupts
	TIM_ClearFlag(TIM14, TIM_FLAG_Update|TIM_FLAG_CC1);
	// Handles interrupt
	if (cc_flags) {
        mal_hspec_stm32f0_timer_input_capture_interrupt(timer14, cc_flags);
	} else {
        if (NULL != timer14 && NULL != timer14->callback) {
            timer14->callback(timer14->handle);
        }
	}
}

void TIM15_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = 0;
	if (TIM15->DIER & TIM_DIER_CC1IE) {
		cc_flags |= TIM15->SR & TIM_FLAG_CC1;
	}
	if (TIM15->DIER & TIM_DIER_CC2IE) {
		cc_flags |= TIM15->SR & TIM_FLAG_CC2;
	}
	// Clear interrupts
	TIM_ClearFlag(TIM15, TIM_FLAG_Update|TIM_FLAG_CC1|TIM_FLAG_CC2);
	// Handles interrupt
	if (cc_flags) {
        mal_hspec_stm32f0_timer_input_capture_interrupt(timer15, cc_flags);
	} else {
        if (NULL != timer15 && NULL != timer15->callback) {
            timer15->callback(timer15->handle);
        }
	}
}

void TIM16_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = 0;
	if (TIM16->DIER & TIM_DIER_CC1IE) {
		cc_flags = TIM16->SR & TIM_FLAG_CC1;
	}
	// Clear interrupts
	TIM_ClearFlag(TIM16, TIM_FLAG_Update|TIM_FLAG_CC1);
	// Handles interrupt
	if (cc_flags) {
        mal_hspec_stm32f0_timer_input_capture_interrupt(timer16, cc_flags);
	} else {
        if (NULL != timer16 && NULL != timer16->callback) {
            timer16->callback(timer16->handle);
        }
	}
}

void TIM17_IRQHandler(void) {
	// Check if this is a capture interrupt
	uint16_t cc_flags = 0;
	if (TIM17->DIER & TIM_DIER_CC1IE) {
		cc_flags = TIM17->SR & TIM_FLAG_CC1;
	}
	// Clear interrupts
	TIM_ClearFlag(TIM17, TIM_FLAG_Update|TIM_FLAG_CC1);
	// Handles interrupt
	if (cc_flags) {
        mal_hspec_stm32f0_timer_input_capture_interrupt(timer17, cc_flags);
	} else {
        if (NULL != timer17 && NULL != timer17->callback) {
            timer17->callback(timer17->handle);
        }
	}
}

mal_error_e mal_timer_free(mal_timer_s *handle) {
	// Disable timer
	TIM_Cmd(handle->stm_handle, DISABLE);
	// Disable interrupt
	TIM_ITConfig(handle->stm_handle, TIM_IT_Update|TIM_FLAG_CC1|TIM_FLAG_CC2|TIM_FLAG_CC3|TIM_FLAG_CC4, DISABLE);
	NVIC_DisableIRQ(handle->irq);
	// Unitialise timer
	TIM_DeInit(handle->stm_handle);
	// Remove handle
    switch (handle->timer) {
        case MAL_TIMER_1:
            timer1 = NULL;
            break;
        case MAL_TIMER_2:
            timer2 = NULL;
            break;
        case MAL_TIMER_3:
            timer3 = NULL;
            break;
        case MAL_TIMER_6:
            timer6 = NULL;
            break;
        case MAL_TIMER_7:
            timer7 = NULL;
            break;
        case MAL_TIMER_14:
            timer14 = NULL;
            break;
        case MAL_TIMER_15:
            timer15 = NULL;
            break;
        case MAL_TIMER_16:
            timer16 = NULL;
            break;
        case MAL_TIMER_17:
            timer17 = NULL;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }

	return MAL_ERROR_OK;
}

MAL_DEFS_INLINE void mal_timer_disable_interrupt(mal_timer_s *handle, mal_timer_interrupt_state_s *state) {
	state->active = NVIC_GetActive(handle->irq);
	NVIC_DisableIRQ(handle->irq);
	__DSB();
	__ISB();
}

MAL_DEFS_INLINE void mal_timer_restore_interrupt(mal_timer_s *handle, mal_timer_interrupt_state_s *state) {
	if (state->active) {
		NVIC_EnableIRQ(handle->irq);
	}
}

uint16_t mal_hspec_stm32f0_timer_get_channel(mal_gpio_port_e port, uint8_t pin, mal_timer_e timer) {
	const mal_hspec_stm32f0_af_e (*timer_afs)[MAL_GPIO_PORT_SIZE][MAL_HSPEC_STM32F0_GPIO_PORT_SIZE][MAL_TIMER_SIZE];
	// Fetch timer alternate functions
	mal_hspec_stm32f0_get_timer_afs(&timer_afs);
	switch ((*timer_afs)[port][pin][timer]) {
		case MAL_HSPEC_STM32F0_AF_TIM1_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM1_CH1N:
		case MAL_HSPEC_STM32F0_AF_TIM2_CH1_ETR:
		case MAL_HSPEC_STM32F0_AF_TIM3_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM14_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM15_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM15_CH1N:
		case MAL_HSPEC_STM32F0_AF_TIM16_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM16_CH1N:
		case MAL_HSPEC_STM32F0_AF_TIM17_CH1:
		case MAL_HSPEC_STM32F0_AF_TIM17_CH1N:
			return TIM_Channel_1;
		case MAL_HSPEC_STM32F0_AF_TIM1_CH2:
		case MAL_HSPEC_STM32F0_AF_TIM1_CH2N:
		case MAL_HSPEC_STM32F0_AF_TIM2_CH2:
		case MAL_HSPEC_STM32F0_AF_TIM3_CH2:
		case MAL_HSPEC_STM32F0_AF_TIM15_CH2:
			return TIM_Channel_2;
		case MAL_HSPEC_STM32F0_AF_TIM1_CH3:
		case MAL_HSPEC_STM32F0_AF_TIM1_CH3N:
		case MAL_HSPEC_STM32F0_AF_TIM2_CH3:
		case MAL_HSPEC_STM32F0_AF_TIM3_CH3:
			return TIM_Channel_3;
		case MAL_HSPEC_STM32F0_AF_TIM1_CH4:
		case MAL_HSPEC_STM32F0_AF_TIM2_CH4:
		case MAL_HSPEC_STM32F0_AF_TIM3_CH4:
			return TIM_Channel_4;
		default:
			return UINT16_MAX;
	}
}

mal_error_e mal_timer_get_resolution(mal_timer_e timer, uint8_t *resolution) {
	switch (timer) {
		case MAL_TIMER_1:
		case MAL_TIMER_3:
		case MAL_TIMER_6:
		case MAL_TIMER_7:
		case MAL_TIMER_14:
		case MAL_TIMER_15:
		case MAL_TIMER_16:
		case MAL_TIMER_17:
			*resolution = 16;
			break;
		case MAL_TIMER_2:
			*resolution = 32;
			break;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_pwm(mal_timer_init_pwm_s *init, mal_timer_s *handle, mal_timer_pwm_s *pwm_handle) {
	mal_error_e result;
	// Get timer
	TIM_TypeDef *tim = mal_hspec_stm32f0_timer_get_typedef(init->timer);
	// Get timer parameters
    TIM_TimeBaseInitTypeDef time_base_init;
    result = mal_hspec_stm32f0_timer_get_update_time_base(&time_base_init, init->frequency, init->delta);
    if (MAL_ERROR_OK != result) {
        return result;
    }
	// Check if the timer is already initialized
	if (!(tim->CR1 & TIM_CR1_CEN)) {
		// Initialize timer
	    result = mal_hspec_stm32f0_timer_common_init(init->timer, &time_base_init, handle);
		if (MAL_ERROR_OK != result) {
			return result;
		}
        // Enable timer
        TIM_Cmd(handle->stm_handle, ENABLE);
	} else {
		// Check parameters
        if (handle->stm_handle->ARR != time_base_init.TIM_Period ||
            handle->stm_handle->PSC != time_base_init.TIM_Prescaler) {
			return MAL_ERROR_HARDWARE_UNAVAILABLE;
		}
	}
	// Enable GPIO clock
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->port), ENABLE);
	// Configure alternate function
	uint8_t function;
	result = mal_hspec_stm32f0_get_timer_af(init->port, init->pin, init->timer, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->port), init->pin, function);
	// Configure GPIO
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->pin);
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->port), &gpio_init);
	// Disable timer while we set the PWM output
	TIM_Cmd(tim, DISABLE);
	// Configure output channel
	TIM_OCInitTypeDef channel_init;
	channel_init.TIM_OCMode = TIM_OCMode_PWM1; // Output is active up to compare
	channel_init.TIM_OutputState = TIM_OutputState_Enable;
	channel_init.TIM_OutputNState = TIM_OutputNState_Enable;
	channel_init.TIM_Pulse = 0; // Duty cycle of 0 at init.
	channel_init.TIM_OCPolarity = TIM_OCPolarity_High;
	channel_init.TIM_OCNPolarity = TIM_OCNPolarity_High;
	channel_init.TIM_OCIdleState = TIM_OCIdleState_Reset;
	channel_init.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	// Chose the correct init and set function
	switch (mal_hspec_stm32f0_timer_get_channel(init->port, init->pin, init->timer)) {
		case TIM_Channel_1:
			TIM_OC1Init(tim, &channel_init);
			pwm_handle->set_compare = &TIM_SetCompare1;
			break;
		case TIM_Channel_2:
			TIM_OC2Init(tim, &channel_init);
            pwm_handle->set_compare = &TIM_SetCompare1;
			break;
		case TIM_Channel_3:
			TIM_OC3Init(tim, &channel_init);
            pwm_handle->set_compare = &TIM_SetCompare1;
			break;
		case TIM_Channel_4:
			TIM_OC4Init(tim, &channel_init);
            pwm_handle->set_compare = &TIM_SetCompare1;
			break;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}
	// Initialize handle
    pwm_handle->timer_handle = handle;
	// Enable timer
	TIM_Cmd(tim, ENABLE);
	// Check if output has to be enabled
	switch (init->timer) {
		case MAL_TIMER_1:
		case MAL_TIMER_15:
		case MAL_TIMER_16:
		case MAL_TIMER_17:
			TIM_CtrlPWMOutputs(tim, ENABLE);
			break;
		default:
			// Nothing to do for other timers
			break;
	}

	return MAL_ERROR_OK;
}

mal_error_e mal_timer_set_pwm_duty_cycle(mal_timer_pwm_s *handle, mal_ratio_t duty_cycle) {
	uint32_t compare_value = MAL_TYPES_RATIO_OF_INT_VALUE(duty_cycle, uint32_t, handle->timer_handle->stm_handle->ARR);
	handle->set_compare(handle->timer_handle->stm_handle, compare_value);

	return MAL_ERROR_OK;
}

mal_error_e mal_timer_fast_set_frequency(mal_timer_s *handle, mal_hertz_t frequency, mal_hertz_t delta) {
    mal_error_e mal_result;
    // Get timer input clock
    uint64_t timer_frequency;
    mal_result = mal_hspec_stm32f0_timer_get_input_clk(&timer_frequency);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    timer_frequency *= 1000;
    // Try to find proper settings for requested frequency
    uint64_t target_frequency = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(frequency);
    uint64_t target_delta = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(delta);
    uint64_t period = handle->stm_handle->ARR;
    uint64_t new_prescaler = timer_frequency / (period * target_frequency);
    if (new_prescaler >= (UINT16_MAX + 1)) {
        return MAL_ERROR_OPERATION_INVALID;
    }
    // Check if the new frequency would be within the delta
    uint64_t potential_frequency = timer_frequency / (period * new_prescaler);
    uint64_t actual_delta;
    if (potential_frequency >= target_frequency) {
        actual_delta = potential_frequency - target_frequency;
    } else {
        actual_delta = target_frequency - potential_frequency;
    }
    if (actual_delta > target_delta) {
        return MAL_ERROR_OPERATION_INVALID;
    }
    // Remove one because register is 0 based.
    new_prescaler -= 1;
    // Set timer
    TIM_TimeBaseInitTypeDef time_base;
    TIM_TimeBaseStructInit(&time_base);
    time_base.TIM_CounterMode = TIM_CounterMode_Up;
    time_base.TIM_Prescaler = (uint16_t)new_prescaler;
    time_base.TIM_Period = period;
    TIM_TimeBaseInit(handle->stm_handle, &time_base);

    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f0_timer_get_count_time_base(TIM_TimeBaseInitTypeDef *time_base,
                                                               mal_timer_e timer,
                                                               mal_hertz_t frequency) {
    mal_error_e result;
    // Get timer frequency
    uint64_t timer_frequency;
    result = mal_hspec_stm32f0_timer_get_input_clk(&timer_frequency);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Initialize time base timer
    TIM_TimeBaseStructInit(time_base);
    time_base->TIM_CounterMode = TIM_CounterMode_Up;
    // Compute prescaler
    uint64_t target_frequency = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(frequency);
    if (0 == target_frequency) {
        return MAL_ERROR_CLOCK_ERROR;
    }
    uint32_t prescaler = (uint32_t)((timer_frequency * 1000ULL) / target_frequency);
    if (prescaler > (UINT16_MAX+ 1)) {
        return MAL_ERROR_CLOCK_ERROR;
    }
    // Round up prescaler to at least 1
    if (0 == prescaler) {
        prescaler += 1;
    }
    time_base->TIM_Prescaler = (uint16_t)(prescaler - 1);
    // Set maximum value
    uint8_t resolution;
    result = mal_timer_get_resolution(timer, &resolution);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    uint32_t max_value = (uint32_t)((1ULL << ((uint64_t)resolution)) - 1ULL);
    time_base->TIM_Period = max_value;

    return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_count(mal_timer_init_count_s *init, mal_timer_s *handle) {
	mal_error_e result;
	// Initialize time base timer
	TIM_TimeBaseInitTypeDef params;
	result = mal_hspec_stm32f0_timer_get_count_time_base(&params, init->timer, init->frequency);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    result = mal_hspec_stm32f0_timer_common_init(init->timer, &params, handle);
    if (MAL_ERROR_OK != result) {
        return result;
    }
	// Enable timer
	TIM_Cmd(handle->stm_handle, ENABLE);

	return MAL_ERROR_OK;
}

mal_error_e mal_timer_get_count_frequency(mal_timer_s *handle, mal_hertz_t *frequency) {
	*frequency = handle->count_frequency;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_get_count(mal_timer_s *handle, uint64_t *count) {
	*count = handle->stm_handle->CNT;
	return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_input_capture(mal_timer_init_intput_capture_s *init, mal_timer_s *handle,
                                         mal_timer_input_capture_s *input_capture_handle) {
	mal_error_e result;
    // Get time base parameters
    TIM_TimeBaseInitTypeDef time_base_init;
    result = mal_hspec_stm32f0_timer_get_count_time_base(&time_base_init, init->timer, init->frequency);
    if (MAL_ERROR_OK != result) {
        return result;
    }
	// Get timer
	TIM_TypeDef *tim = mal_hspec_stm32f0_timer_get_typedef(init->timer);
	// Check if the timer is already initialized
	if (!(tim->CR1 & TIM_CR1_CEN)) {
		// Initialize timer
        result = mal_hspec_stm32f0_timer_common_init(init->timer, &time_base_init, handle);
        if (MAL_ERROR_OK != result) {
            return result;
        }
        // Enable timer
        TIM_Cmd(handle->stm_handle, ENABLE);
	} else {
        // Check parameters
        if (handle->stm_handle->ARR != time_base_init.TIM_Period ||
            handle->stm_handle->PSC != time_base_init.TIM_Prescaler) {
            return MAL_ERROR_HARDWARE_UNAVAILABLE;
        }
	}
	// Enable GPIO clock
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->port), ENABLE);
	// Configure alternate function
	uint8_t function;
	result = mal_hspec_stm32f0_get_timer_af(init->port, init->pin, init->timer, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->port), init->pin, function);
	// Configure GPIO
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->pin);
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->port), &gpio_init);
	// Disable timer while we set the IO
	TIM_Cmd(tim, DISABLE);
	// Get timer channel
	uint16_t timer_channel = mal_hspec_stm32f0_timer_get_channel(init->port, init->pin, init->timer);
	uint16_t ic_index = (uint16_t)(timer_channel / 4);
	// Configure input channel
	TIM_ICInitTypeDef input_capture_init;
	TIM_ICStructInit(&input_capture_init);
	input_capture_init.TIM_ICSelection = TIM_ICSelection_DirectTI;
	input_capture_init.TIM_ICFilter = 0; // No filter
	// Set channel
	input_capture_init.TIM_Channel = timer_channel;
	// Set requested polarity
	switch (init->input_event) {
		case MAL_TIMER_INPUT_FALLING:
			input_capture_init.TIM_ICPolarity = TIM_ICPolarity_Falling;
			break;
		case MAL_TIMER_INPUT_RISING:
			input_capture_init.TIM_ICPolarity = TIM_ICPolarity_Rising;
			break;
		case MAL_TIMER_INPUT_BOTH:
		default:
			input_capture_init.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
			break;
	}
	// Set input prescaler
	switch (init->input_divider) {
		case 1:
			input_capture_init.TIM_ICPrescaler = TIM_ICPSC_DIV1;
			break;
		case 2:
			input_capture_init.TIM_ICPrescaler = TIM_ICPSC_DIV2;
			break;
		case 4:
			input_capture_init.TIM_ICPrescaler = TIM_ICPSC_DIV4;
			break;
		case 8:
			input_capture_init.TIM_ICPrescaler = TIM_ICPSC_DIV8;
			break;
		default:
			return MAL_ERROR_HARDWARE_INVALID;
	}
	// Initialize input
	TIM_ICInit(tim, &input_capture_init);
	// Save handles
    handle->irq = mal_hspec_stm32f0_get_timer_compare_irq(init->timer);
    handle->input_capture_handles[ic_index] = input_capture_handle;
    input_capture_handle->callback = init->callback;
    input_capture_handle->callback_handle = init->callback_handle;
	// Enable NVIC interrupt for timer
	if (NULL != init->callback) {
		NVIC_EnableIRQ(handle->irq);
		NVIC_SetPriority(handle->irq, 2); // MAL Issue #19 Find a way to manage priorities for interrupts
		// Enable timer interrupt
		uint16_t tim_interrupt;
		switch (timer_channel) {
			case TIM_Channel_1:
				tim_interrupt = TIM_IT_CC1;
				break;
			case TIM_Channel_2:
				tim_interrupt = TIM_IT_CC2;
				break;
			case TIM_Channel_3:
				tim_interrupt = TIM_IT_CC3;
				break;
			case TIM_Channel_4:
			default:
				tim_interrupt = TIM_IT_CC4;
				break;
		}
		TIM_ITConfig(tim, tim_interrupt, ENABLE);
	}
	// Enable timer
	TIM_Cmd(tim, ENABLE);

	return MAL_ERROR_OK;
}

static void mal_hspec_stm32f0_timer_input_capture_interrupt(mal_timer_s *handle, uint16_t flags) {
	// Check every channel and execute callbacks
	if (flags & TIM_FLAG_CC1) {
		if (handle->input_capture_handles[0]->callback != NULL) {
            handle->input_capture_handles[0]->callback(handle->input_capture_handles[0]->callback_handle,
                                                       handle->stm_handle->CCR1);
		}
	}
	if (flags & TIM_FLAG_CC2) {
		if (handle->input_capture_handles[1]->callback != NULL) {
            handle->input_capture_handles[1]->callback(handle->input_capture_handles[1]->callback_handle,
                                                       handle->stm_handle->CCR2);
		}
	}
	if (flags & TIM_FLAG_CC3) {
		if (handle->input_capture_handles[2]->callback != NULL) {
            handle->input_capture_handles[2]->callback(handle->input_capture_handles[2]->callback_handle,
                                                       handle->stm_handle->CCR3);
		}
	}
	if (flags & TIM_FLAG_CC4) {
		if (handle->input_capture_handles[3]->callback != NULL) {
            handle->input_capture_handles[3]->callback(handle->input_capture_handles[3]->callback_handle,
                                                       handle->stm_handle->CCR4);
		}
	}
}
