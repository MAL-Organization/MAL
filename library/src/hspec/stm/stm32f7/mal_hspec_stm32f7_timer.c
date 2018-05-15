/*
 * mal_hspec_stm32f7_timer.c
 *
 *  Created on: May 7, 2018
 *      Author: olivi
 */
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

#include "gpio/mal_gpio.h"
#include "timer/mal_timer2.h"
#include "stm32f7/stm32f7xx_hal_gpio.h"
#include "mal_hspec_stm32f7_timer.h"
#include "stm32f7/stm32f7xx_hal_rcc.h"
#include "clock/mal_clock.h"
#include "stm32f7/stm32f7xx_hal_gpio_ex.h"
#include "mal_hspec_stm32f7_gpio.h"

static IRQn_Type mal_hspec_stm32f7_timer_get_update_irq(mal_timer_e timer);
static mal_error_e mal_hspec_stm32f7_timer_get_input_clk(mal_timer_e timer, uint64_t *clock);
static void mal_hspec_stm32f7_timer_handle_update(mal_timer_s *handle);
static mal_error_e mal_hspec_stm32f7_timer_common_init(mal_timer_e timer, mal_timer_s *handle, mal_hertz_t frequency,
                                                       mal_hertz_t delta);
static mal_error_e mal_hspec_stm32f7_timer_get_channel(mal_timer_e timer, const mal_gpio_s *gpio, uint32_t *channel);
static mal_error_e mal_hspec_stm32f7_timer_get_alternate(mal_timer_e timer, uint32_t *alternate);

static const mal_timer_e valid_timers[] = {
    MAL_TIMER_1,
    MAL_TIMER_2,
    MAL_TIMER_3,
    MAL_TIMER_4,
    MAL_TIMER_5,
    MAL_TIMER_6,
    MAL_TIMER_7,
    MAL_TIMER_8,
    MAL_TIMER_9,
    MAL_TIMER_10,
    MAL_TIMER_11,
    MAL_TIMER_12,
    MAL_TIMER_13,
    MAL_TIMER_14
};

static mal_timer_s *timer1_handle;
static mal_timer_s *timer2_handle;
static mal_timer_s *timer3_handle;
static mal_timer_s *timer4_handle;
static mal_timer_s *timer5_handle;
static mal_timer_s *timer6_handle;
static mal_timer_s *timer7_handle;
static mal_timer_s *timer8_handle;
static mal_timer_s *timer9_handle;
static mal_timer_s *timer10_handle;
static mal_timer_s *timer11_handle;
static mal_timer_s *timer12_handle;
static mal_timer_s *timer13_handle;
static mal_timer_s *timer14_handle;

mal_error_e mal_timer_get_valid_timers(const mal_timer_e **timers, uint8_t *size) {
    *timers = valid_timers;
    *size = sizeof(valid_timers) / sizeof(mal_timer_e);
    return MAL_ERROR_OK;
}

static IRQn_Type mal_hspec_stm32f7_timer_get_update_irq(mal_timer_e timer) {
    switch (timer) {
        case MAL_TIMER_1:
        case MAL_TIMER_10:
            return TIM1_UP_TIM10_IRQn;
        case MAL_TIMER_2:
            return TIM2_IRQn;
        case MAL_TIMER_3:
            return TIM3_IRQn;
        case MAL_TIMER_4:
            return TIM4_IRQn;
        case MAL_TIMER_5:
            return TIM5_IRQn;
        case MAL_TIMER_6:
            return TIM6_DAC_IRQn;
        case MAL_TIMER_7:
            return TIM7_IRQn;
        case MAL_TIMER_8:
        case MAL_TIMER_13:
            return TIM8_UP_TIM13_IRQn;
        case MAL_TIMER_9:
            return TIM1_BRK_TIM9_IRQn;
        case MAL_TIMER_11:
            return TIM1_TRG_COM_TIM11_IRQn;
        case MAL_TIMER_12:
            return TIM8_BRK_TIM12_IRQn;
        case MAL_TIMER_14:
        default:
            return TIM8_TRG_COM_TIM14_IRQn;
    }
}

#include "stm32f7/stm32f7xx_hal_cortex.h"
MAL_DEFS_INLINE bool mal_timer_disable_interrupt(mal_timer_s *handle) {
    bool active = (bool)NVIC_GetEnableIRQ(handle->update_irq);
    NVIC_DisableIRQ(handle->update_irq);
    __DSB();
    __ISB();
    return active;
}

MAL_DEFS_INLINE void mal_timer_enable_interrupt(mal_timer_s *handle, bool active) {
    if (active) {
        NVIC_EnableIRQ(handle->update_irq);
    }
}

mal_error_e mal_timer_get_resolution(mal_timer_e timer, uint8_t *resolution) {
    switch (timer) {
        case MAL_TIMER_1:
        case MAL_TIMER_3:
        case MAL_TIMER_4:
        case MAL_TIMER_6:
        case MAL_TIMER_7:
        case MAL_TIMER_8:
        case MAL_TIMER_9:
        case MAL_TIMER_10:
        case MAL_TIMER_11:
        case MAL_TIMER_12:
        case MAL_TIMER_13:
        case MAL_TIMER_14:
            *resolution = 16;
            break;
        case MAL_TIMER_2:
        case MAL_TIMER_5:
            *resolution = 32;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f7_timer_get_input_clk(mal_timer_e timer, uint64_t *clock) {
    // Get clock configurations
    RCC_ClkInitTypeDef clock_config;
    uint32_t flash_latency;
    HAL_RCC_GetClockConfig(&clock_config, &flash_latency);
    // Get timer PCLK and apb prescaler
    uint64_t pclk;
    uint32_t apb_prescaler;
    switch (timer) {
        case MAL_TIMER_1:
        case MAL_TIMER_8:
        case MAL_TIMER_9:
        case MAL_TIMER_10:
        case MAL_TIMER_11:
            // Save as mHz
            pclk = (uint64_t)HAL_RCC_GetPCLK2Freq() * 1000ULL;
            apb_prescaler = clock_config.APB2CLKDivider;
            break;
        case MAL_TIMER_2:
        case MAL_TIMER_3:
        case MAL_TIMER_4:
        case MAL_TIMER_5:
        case MAL_TIMER_6:
        case MAL_TIMER_7:
        case MAL_TIMER_12:
        case MAL_TIMER_13:
        case MAL_TIMER_14:
            // Save as mHz
            pclk = (uint64_t)HAL_RCC_GetPCLK1Freq() * 1000ULL;
            apb_prescaler = clock_config.APB1CLKDivider;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    // Get timer prescaler settings
    uint32_t rcc_tim_prescaler_mode = RCC->DCKCFGR1 & RCC_DCKCFGR1_DFSDM1SEL_Msk;
    // Get system clock
    mal_hertz_t system_clock = mal_clock_get_system_clock();
    uint64_t system_clock_value = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(system_clock);
    // Compute proper input value
    if (rcc_tim_prescaler_mode) {
        switch (apb_prescaler) {
            case RCC_HCLK_DIV1:
            case RCC_HCLK_DIV2:
            case RCC_HCLK_DIV4:
                *clock = system_clock_value;
                break;
            default:
                *clock = pclk * 4;
                break;
        }
    } else {
        if (RCC_HCLK_DIV1 == apb_prescaler) {
            *clock = pclk;
        } else {
            *clock = pclk * 2;
        }
    }

    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f7_timer_common_init(mal_timer_e timer, mal_timer_s *handle, mal_hertz_t frequency,
                                                       mal_hertz_t delta) {
    mal_error_e mal_result;
    // Fetch local timer handle
    mal_timer_s *local_handle;
    switch (timer) {
        case MAL_TIMER_1:
            local_handle = timer1_handle;
            break;
        case MAL_TIMER_2:
            local_handle = timer2_handle;
            break;
        case MAL_TIMER_3:
            local_handle = timer3_handle;
            break;
        case MAL_TIMER_4:
            local_handle = timer4_handle;
            break;
        case MAL_TIMER_5:
            local_handle = timer5_handle;
            break;
        case MAL_TIMER_6:
            local_handle = timer6_handle;
            break;
        case MAL_TIMER_7:
            local_handle = timer7_handle;
            break;
        case MAL_TIMER_8:
            local_handle = timer8_handle;
            break;
        case MAL_TIMER_9:
            local_handle = timer9_handle;
            break;
        case MAL_TIMER_10:
            local_handle = timer10_handle;
            break;
        case MAL_TIMER_11:
            local_handle = timer11_handle;
            break;
        case MAL_TIMER_12:
            local_handle = timer12_handle;
            break;
        case MAL_TIMER_13:
            local_handle = timer13_handle;
            break;
        case MAL_TIMER_14:
            local_handle = timer14_handle;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    // Check if timer is already initialized
    bool validate_parameters = false;
    if (NULL != local_handle) {
        validate_parameters = true;
    }
    // Initialize clock and timer specific handles handles
    switch (timer) {
        case MAL_TIMER_1:
            __HAL_RCC_TIM1_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM1;
            timer1_handle = handle;
            break;
        case MAL_TIMER_2:
            __HAL_RCC_TIM2_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM2;
            timer2_handle = handle;
            break;
        case MAL_TIMER_3:
            __HAL_RCC_TIM3_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM3;
            timer3_handle = handle;
            break;
        case MAL_TIMER_4:
            __HAL_RCC_TIM4_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM4;
            timer4_handle = handle;
            break;
        case MAL_TIMER_5:
            __HAL_RCC_TIM5_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM5;
            timer5_handle = handle;
            break;
        case MAL_TIMER_6:
            __HAL_RCC_TIM6_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM6;
            timer6_handle = handle;
            break;
        case MAL_TIMER_7:
            __HAL_RCC_TIM7_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM7;
            timer7_handle = handle;
            break;
        case MAL_TIMER_8:
            __HAL_RCC_TIM8_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM8;
            timer8_handle = handle;
            break;
        case MAL_TIMER_9:
            __HAL_RCC_TIM9_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM9;
            timer9_handle = handle;
            break;
        case MAL_TIMER_10:
            __HAL_RCC_TIM10_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM10;
            timer10_handle = handle;
            break;
        case MAL_TIMER_11:
            __HAL_RCC_TIM11_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM11;
            timer11_handle = handle;
            break;
        case MAL_TIMER_12:
            __HAL_RCC_TIM12_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM12;
            timer12_handle = handle;
            break;
        case MAL_TIMER_13:
            __HAL_RCC_TIM13_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM13;
            timer13_handle = handle;
            break;
        case MAL_TIMER_14:
        default:
            __HAL_RCC_TIM14_CLK_ENABLE();
            handle->hal_timer_handle.Instance = TIM14;
            timer14_handle = handle;
            break;
    }
    // Get timer input clock
    uint64_t timer_frequency;
    mal_result = mal_hspec_stm32f7_timer_get_input_clk(timer, &timer_frequency);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Fetch mask to get max period
    uint64_t mask;
    mal_result = mal_timer_get_count_mask(timer, &mask);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Try to find proper settings for requested frequency
    uint64_t target_frequency = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(frequency);
    uint64_t target_delta = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(delta);
    uint32_t prescaler = 0;
    uint32_t period;
    bool found = false;
    uint64_t smallest_delta = UINT64_MAX;
    for (period = (uint32_t)mask; period > 0; period--) {
        for (prescaler = 1; prescaler <= (UINT16_MAX + 1); prescaler++) {
            uint64_t potential_frequency = timer_frequency / ((uint64_t)period * (uint64_t)prescaler);
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
    // Remove one because register is 0 based.
    prescaler -= 1;
    // Validate or set
    if (validate_parameters) {
        if ((handle->hal_timer_handle.Init.Prescaler != prescaler) ||
            (handle->hal_timer_handle.Init.Period != period)) {
            return MAL_ERROR_HARDWARE_UNAVAILABLE;
        }
        return MAL_ERROR_ALREADY_INITIALIZED;
    } else {
        // Set timer
        handle->hal_timer_handle.Init.Prescaler = prescaler;
        handle->hal_timer_handle.Init.Period = period;
        handle->hal_timer_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
        handle->hal_timer_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        handle->hal_timer_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        handle->hal_timer_handle.Init.RepetitionCounter = 0;
        handle->timer = timer;
    }

    return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_task(mal_timer_init_task_s *init, mal_timer_s *handle) {
    mal_error_e mal_result;
    HAL_StatusTypeDef hal_result;
    // Common initialisation such as clock, local handles and more.
    mal_result = mal_hspec_stm32f7_timer_common_init(init->timer, handle, init->frequency, init->delta);
    if (MAL_ERROR_OK != mal_result && MAL_ERROR_ALREADY_INITIALIZED != mal_result) {
        return mal_result;
    }
    // Initialise timer in basic mode
    if (MAL_ERROR_ALREADY_INITIALIZED == mal_result) {
        if (MAL_HSPEC_STM32F7_TIMER_MODE_BASIC != handle->mode) {
            return MAL_ERROR_HARDWARE_UNAVAILABLE;
        }
    } else {
        hal_result = HAL_TIM_Base_Init(&handle->hal_timer_handle);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_HARDWARE_INVALID;
        }
        handle->mode = MAL_HSPEC_STM32F7_TIMER_MODE_BASIC;
    }
    // Enable NVIC interrupt for timer
    handle->update_irq = mal_hspec_stm32f7_timer_get_update_irq(init->timer);
    NVIC_EnableIRQ(handle->update_irq);
    NVIC_SetPriority(handle->update_irq, 2); // Find a way to manage priorities for interrupts
    // Enable timer interrupt
    handle->task_callback = init->callback;
    handle->callback_handle = init->callback_handle;
    handle->hal_timer_handle.Parent = handle;
    HAL_TIM_Base_Start_IT(&handle->hal_timer_handle);
}

static mal_error_e mal_hspec_stm32f7_timer_get_channel(mal_timer_e timer, const mal_gpio_s *gpio, uint32_t *channel) {
    switch (timer) {
        case MAL_TIMER_1:
            if ((MAL_GPIO_PORT_A == gpio->port && 7 == gpio->pin) ||
                (MAL_GPIO_PORT_E == gpio->port && 8 == gpio->pin) ||
                (MAL_GPIO_PORT_E == gpio->port && 9 == gpio->pin) ||
                (MAL_GPIO_PORT_B == gpio->port && 13 == gpio->pin) ||
                (MAL_GPIO_PORT_A == gpio->port && 8 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == gpio->port && 0 == gpio->pin) ||
                       (MAL_GPIO_PORT_E == gpio->port && 10 == gpio->pin) ||
                       (MAL_GPIO_PORT_E == gpio->port && 11 == gpio->pin) ||
                       (MAL_GPIO_PORT_B == gpio->port && 14 == gpio->pin) ||
                       (MAL_GPIO_PORT_A == gpio->port && 9 == gpio->pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == gpio->port && 1 == gpio->pin) ||
                       (MAL_GPIO_PORT_E == gpio->port && 12 == gpio->pin) ||
                       (MAL_GPIO_PORT_E == gpio->port && 13 == gpio->pin) ||
                       (MAL_GPIO_PORT_B == gpio->port && 15 == gpio->pin) ||
                       (MAL_GPIO_PORT_A == gpio->port && 10 == gpio->pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_E == gpio->port && 14 == gpio->pin) ||
                       (MAL_GPIO_PORT_A == gpio->port && 11 == gpio->pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_2:
            if ((MAL_GPIO_PORT_A == gpio->port && 0 == gpio->pin) ||
                (MAL_GPIO_PORT_A == gpio->port && 5 == gpio->pin) ||
                (MAL_GPIO_PORT_A == gpio->port && 15 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == gpio->port && 1 == gpio->pin) ||
                       (MAL_GPIO_PORT_B == gpio->port && 3 == gpio->pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == gpio->port && 2 == gpio->pin) ||
                       (MAL_GPIO_PORT_B == gpio->port && 10 == gpio->pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == gpio->port && 3 == gpio->pin) ||
                       (MAL_GPIO_PORT_B == gpio->port && 11 == gpio->pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_3:
            if ((MAL_GPIO_PORT_A == gpio->port && 6 == gpio->pin) ||
                (MAL_GPIO_PORT_C == gpio->port && 6 == gpio->pin) ||
                (MAL_GPIO_PORT_B == gpio->port && 4 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == gpio->port && 7 == gpio->pin) ||
                       (MAL_GPIO_PORT_C == gpio->port && 7 == gpio->pin) ||
                       (MAL_GPIO_PORT_B == gpio->port && 5 == gpio->pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == gpio->port && 0 == gpio->pin) ||
                       (MAL_GPIO_PORT_C == gpio->port && 8 == gpio->pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == gpio->port && 1 == gpio->pin) ||
                       (MAL_GPIO_PORT_C == gpio->port && 9 == gpio->pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_4:
            if ((MAL_GPIO_PORT_D == gpio->port && 12 == gpio->pin) ||
                (MAL_GPIO_PORT_B == gpio->port && 6 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_D == gpio->port && 13 == gpio->pin) ||
                       (MAL_GPIO_PORT_B == gpio->port && 7 == gpio->pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_D == gpio->port && 14 == gpio->pin)  ||
                       (MAL_GPIO_PORT_B == gpio->port && 8 == gpio->pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_D == gpio->port && 15 == gpio->pin) ||
                       (MAL_GPIO_PORT_B == gpio->port && 9 == gpio->pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_5:
            if ((MAL_GPIO_PORT_A == gpio->port && 0 == gpio->pin) ||
                (MAL_GPIO_PORT_H == gpio->port && 10 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == gpio->port && 1 == gpio->pin) ||
                       (MAL_GPIO_PORT_H == gpio->port && 11 == gpio->pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == gpio->port && 2 == gpio->pin) ||
                       (MAL_GPIO_PORT_H == gpio->port && 12 == gpio->pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == gpio->port && 3 == gpio->pin) ||
                       (MAL_GPIO_PORT_I == gpio->port && 0 == gpio->pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_8:
            if ((MAL_GPIO_PORT_A == gpio->port && 5 == gpio->pin) ||
                (MAL_GPIO_PORT_A == gpio->port && 7 == gpio->pin) ||
                (MAL_GPIO_PORT_C == gpio->port && 6 == gpio->pin) ||
                (MAL_GPIO_PORT_H == gpio->port && 13 == gpio->pin) ||
                (MAL_GPIO_PORT_I == gpio->port && 5 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == gpio->port && 0 == gpio->pin) ||
                       (MAL_GPIO_PORT_B == gpio->port && 14 == gpio->pin) ||
                       (MAL_GPIO_PORT_C == gpio->port && 7 == gpio->pin) ||
                       (MAL_GPIO_PORT_H == gpio->port && 14 == gpio->pin) ||
                       (MAL_GPIO_PORT_I == gpio->port && 6 == gpio->pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == gpio->port && 1 == gpio->pin) ||
                       (MAL_GPIO_PORT_B == gpio->port && 15 == gpio->pin) ||
                       (MAL_GPIO_PORT_C == gpio->port && 8 == gpio->pin) ||
                       (MAL_GPIO_PORT_H == gpio->port && 15 == gpio->pin) ||
                       (MAL_GPIO_PORT_I == gpio->port && 7 == gpio->pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_C == gpio->port && 9 == gpio->pin) ||
                       (MAL_GPIO_PORT_I == gpio->port && 2 == gpio->pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_9:
            if ((MAL_GPIO_PORT_E == gpio->port && 5 == gpio->pin) ||
                (MAL_GPIO_PORT_A == gpio->port && 2 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_E == gpio->port && 6 == gpio->pin) ||
                       (MAL_GPIO_PORT_A == gpio->port && 3 == gpio->pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_10:
            if ((MAL_GPIO_PORT_F == gpio->port && 6 == gpio->pin) ||
                (MAL_GPIO_PORT_B == gpio->port && 8 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_11:
            if ((MAL_GPIO_PORT_F == gpio->port && 7 == gpio->pin) ||
                (MAL_GPIO_PORT_B == gpio->port && 9 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_12:
            if ((MAL_GPIO_PORT_H == gpio->port && 6 == gpio->pin) ||
                (MAL_GPIO_PORT_B == gpio->port && 14 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_H == gpio->port && 9 == gpio->pin) ||
                       (MAL_GPIO_PORT_B == gpio->port && 15 == gpio->pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_13:
            if ((MAL_GPIO_PORT_F == gpio->port && 8 == gpio->pin) ||
                (MAL_GPIO_PORT_A == gpio->port && 6 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_14:
            if ((MAL_GPIO_PORT_F == gpio->port && 9 == gpio->pin) ||
                (MAL_GPIO_PORT_A == gpio->port && 7 == gpio->pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
}

static mal_error_e mal_hspec_stm32f7_timer_get_alternate(mal_timer_e timer, uint32_t *alternate) {
    switch (timer) {
        case MAL_TIMER_1:
            *alternate = GPIO_AF1_TIM1;
            return MAL_ERROR_OK;
        case MAL_TIMER_2:
            *alternate = GPIO_AF1_TIM2;
            return MAL_ERROR_OK;
        case MAL_TIMER_3:
            *alternate = GPIO_AF2_TIM3;
            return MAL_ERROR_OK;
        case MAL_TIMER_4:
            *alternate = GPIO_AF2_TIM4;
            return MAL_ERROR_OK;
        case MAL_TIMER_5:
            *alternate = GPIO_AF2_TIM5;
            return MAL_ERROR_OK;
        case MAL_TIMER_8:
            *alternate = GPIO_AF3_TIM8;
            return MAL_ERROR_OK;
        case MAL_TIMER_9:
            *alternate = GPIO_AF3_TIM9;
            return MAL_ERROR_OK;
        case MAL_TIMER_10:
            *alternate = GPIO_AF3_TIM10;
            return MAL_ERROR_OK;
        case MAL_TIMER_11:
            *alternate = GPIO_AF3_TIM11;
            return MAL_ERROR_OK;
        case MAL_TIMER_12:
            *alternate = GPIO_AF9_TIM12;
            return MAL_ERROR_OK;
        case MAL_TIMER_13:
            *alternate = GPIO_AF9_TIM13;
            return MAL_ERROR_OK;
        case MAL_TIMER_14:
            *alternate = GPIO_AF9_TIM14;
            return MAL_ERROR_OK;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
}

mal_error_e mal_timer_init_pwm(mal_timer_init_pwm_s *init, mal_timer_s *handle, mal_timer_pwm_s *pwm_handle) {
    mal_error_e mal_result;
    HAL_StatusTypeDef hal_result;
    // Common initialisation such as clock, local handles and more.
    mal_error_e common_result;
    common_result = mal_hspec_stm32f7_timer_common_init(init->timer, handle, init->frequency, init->delta);
    if (MAL_ERROR_OK != common_result && MAL_ERROR_ALREADY_INITIALIZED != common_result) {
        return common_result;
    }
    // Enable PWM IO clock
    mal_result = mal_hspec_stm32f7_gpio_enable_clock(init->pwm_io->port);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Set IO in PWM mode
    GPIO_TypeDef *hal_port = mal_hspec_stm32f7_gpio_get_hal_port(init->pwm_io->port);
    GPIO_InitTypeDef gpio_init;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init.Pin = MAL_HSPEC_STM32F7_GPIO_GET_HAL_PIN(init->pwm_io->pin);
    mal_result = mal_hspec_stm32f7_timer_get_alternate(init->timer, &gpio_init.Alternate);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    HAL_GPIO_Init(hal_port, &gpio_init);
    // Initialise timer in PWM mode
    if (MAL_ERROR_ALREADY_INITIALIZED == common_result) {
        if (MAL_HSPEC_STM32F7_TIMER_MODE_PWM != handle->mode) {
            return MAL_ERROR_HARDWARE_UNAVAILABLE;
        }
    } else {
        hal_result = HAL_TIM_PWM_Init(&handle->hal_timer_handle);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_HARDWARE_INVALID;
        }
        handle->mode = MAL_HSPEC_STM32F7_TIMER_MODE_PWM;
    }
    // Set PWM handle
    pwm_handle->handle = handle;
    // Get channel
    mal_result = mal_hspec_stm32f7_timer_get_channel(init->timer, init->pwm_io, &pwm_handle->channel);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Configure channel
    mal_result = mal_timer_set_pwm_duty_cycle(pwm_handle, MAL_TYPES_FLOAT_RATIO_TO_RATIO(0.0f, 1.0f));
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }

    return MAL_ERROR_OK;
}

mal_error_e mal_timer_set_pwm_duty_cycle(mal_timer_pwm_s *handle, mal_ratio_t duty_cycle) {
    HAL_StatusTypeDef hal_result;
    TIM_OC_InitTypeDef oc_config;
    // Stop PWM for update
    HAL_TIM_PWM_Stop(&handle->handle->hal_timer_handle, handle->channel);
    // Update config
    oc_config.OCMode = TIM_OCMODE_PWM1;
    oc_config.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_config.OCNPolarity = TIM_OCPOLARITY_HIGH;
    oc_config.OCIdleState = TIM_OCNIDLESTATE_RESET;
    oc_config.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    oc_config.OCFastMode = TIM_OCFAST_DISABLE;
    oc_config.Pulse = MAL_TYPES_RATIO_OF_INT_VALUE(duty_cycle, uint32_t,
            handle->handle->hal_timer_handle.Instance->ARR);
    hal_result = HAL_TIM_PWM_ConfigChannel(&handle->handle->hal_timer_handle, &oc_config, handle->channel);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    // Start PWM
    hal_result = HAL_TIM_PWM_Start(&handle->handle->hal_timer_handle, handle->channel);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_timer_get_count_frequency(mal_timer_s *handle, mal_hertz_t *frequency) {
    mal_error_e result;
    // Get timer frequency
    uint64_t timer_frequency;
    result = mal_hspec_stm32f7_timer_get_input_clk(handle->timer, &timer_frequency);
    if (MAL_ERROR_OK != result) {
        return result;
    }
    // Compute frequency
    timer_frequency /= (handle->hal_timer_handle.Instance->PSC + 1);
    *frequency = MAL_TYPES_MILLIHERTZ_TO_MAL_HERTZ(timer_frequency);

    return MAL_ERROR_OK;
}

void TIM1_BRK_TIM9_IRQHandler(void) {
    if (NULL != timer1_handle) {
        HAL_TIM_IRQHandler(&timer1_handle->hal_timer_handle);
    }
    if (NULL != timer9_handle) {
        HAL_TIM_IRQHandler(&timer9_handle->hal_timer_handle);
    }
}

void TIM1_UP_TIM10_IRQHandler(void) {
    if (NULL != timer1_handle) {
        HAL_TIM_IRQHandler(&timer1_handle->hal_timer_handle);
    }
    if (NULL != timer10_handle) {
        HAL_TIM_IRQHandler(&timer10_handle->hal_timer_handle);
    }
}

void TIM1_TRG_COM_TIM11_IRQHandler(void) {
    if (NULL != timer1_handle) {
        HAL_TIM_IRQHandler(&timer1_handle->hal_timer_handle);
    }
    if (NULL != timer11_handle) {
        HAL_TIM_IRQHandler(&timer11_handle->hal_timer_handle);
    }
}

void TIM1_CC_IRQHandler(void) {
    if (NULL != timer1_handle) {
        HAL_TIM_IRQHandler(&timer1_handle->hal_timer_handle);
    }
}

void TIM2_IRQHandler(void) {
    if (NULL != timer2_handle) {
        HAL_TIM_IRQHandler(&timer2_handle->hal_timer_handle);
    }
}

void TIM3_IRQHandler(void) {
    if (NULL != timer3_handle) {
        HAL_TIM_IRQHandler(&timer3_handle->hal_timer_handle);
    }
}

void TIM4_IRQHandler(void) {
    if (NULL != timer4_handle) {
        HAL_TIM_IRQHandler(&timer4_handle->hal_timer_handle);
    }
}

void TIM5_IRQHandler(void) {
    if (NULL != timer5_handle) {
        HAL_TIM_IRQHandler(&timer5_handle->hal_timer_handle);
    }
}

void TIM6_DAC_IRQHandler(void) {
    if (NULL != timer6_handle) {
        HAL_TIM_IRQHandler(&timer6_handle->hal_timer_handle);
    }
}

void TIM7_IRQHandler(void) {
    if (NULL != timer7_handle) {
        HAL_TIM_IRQHandler(&timer7_handle->hal_timer_handle);
    }
}

void TIM8_BRK_TIM12_IRQHandler(void) {
    if (NULL != timer8_handle) {
        HAL_TIM_IRQHandler(&timer8_handle->hal_timer_handle);
    }
    if (NULL != timer12_handle) {
        HAL_TIM_IRQHandler(&timer12_handle->hal_timer_handle);
    }
}

void TIM8_UP_TIM13_IRQHandler(void) {
    if (NULL != timer8_handle) {
        HAL_TIM_IRQHandler(&timer8_handle->hal_timer_handle);
    }
    if (NULL != timer13_handle) {
        HAL_TIM_IRQHandler(&timer13_handle->hal_timer_handle);
    }
}

void TIM8_TRG_COM_TIM14_IRQHandler(void) {
    if (NULL != timer8_handle) {
        HAL_TIM_IRQHandler(&timer8_handle->hal_timer_handle);
    }
    if (NULL != timer14_handle) {
        HAL_TIM_IRQHandler(&timer14_handle->hal_timer_handle);
    }
}

void TIM8_CC_IRQHandler(void) {
    if (NULL != timer8_handle) {
        HAL_TIM_IRQHandler(&timer8_handle->hal_timer_handle);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    mal_hspec_stm32f7_timer_handle_update(htim->Parent);
}

static void mal_hspec_stm32f7_timer_handle_update(mal_timer_s *handle) {
    handle->task_callback(handle->callback_handle);
}
