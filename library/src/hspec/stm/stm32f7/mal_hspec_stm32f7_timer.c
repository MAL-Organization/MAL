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

#include "gpio/mal_gpio_definitions.h"
#include "timer/mal_timer.h"
#include "stm32f7/stm32f7xx_hal_gpio.h"
#include "mal_hspec_stm32f7_timer.h"
#include "stm32f7/stm32f7xx_hal_rcc.h"
#include "clock/mal_clock.h"
#include "stm32f7/stm32f7xx_hal_gpio_ex.h"
#include "mal_hspec_stm32f7_gpio.h"

static IRQn_Type mal_hspec_stm32f7_timer_get_update_irq(mal_timer_e timer);
static mal_error_e mal_hspec_stm32f7_timer_get_input_clk(mal_timer_e timer, uint64_t *clock);
static mal_error_e mal_hspec_stm32f7_timer_common_init(mal_timer_e timer, mal_timer_s *handle);
static mal_error_e mal_hspec_stm32f7_timer_common_update_init(mal_timer_e timer, mal_timer_s *handle,
                                                              mal_hertz_t frequency, mal_hertz_t delta);
static mal_error_e mal_hspec_stm32f7_timer_get_channel(mal_timer_e timer, mal_gpio_port_e port, uint8_t pin,
                                                       uint32_t *channel);
static mal_error_e mal_hspec_stm32f7_timer_get_alternate(mal_timer_e timer, uint32_t *alternate);
static bool mal_hspec_stm32f7_timer_is_used(mal_timer_e timer);
static mal_error_e mal_hspec_stm32f7_timer_common_count_init(mal_timer_e timer, mal_timer_s *handle,
                                                             mal_hertz_t frequency);
static mal_error_e mal_hspec_stm32f7_timer_init_io(mal_timer_e timer, mal_gpio_port_e port, uint8_t pin);
static IRQn_Type mal_hspec_stm32f7_timer_get_compare_irq(mal_timer_e timer);

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

static IRQn_Type mal_hspec_stm32f7_timer_get_compare_irq(mal_timer_e timer) {
    switch (timer) {
        case MAL_TIMER_1:
            return TIM1_CC_IRQn;
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
            return TIM8_CC_IRQn;
        case MAL_TIMER_9:
            return TIM1_BRK_TIM9_IRQn;
        case MAL_TIMER_10:
            return TIM1_UP_TIM10_IRQn;
        case MAL_TIMER_11:
            return TIM1_TRG_COM_TIM11_IRQn;
        case MAL_TIMER_12:
            return TIM8_BRK_TIM12_IRQn;
        case MAL_TIMER_13:
            return TIM8_UP_TIM13_IRQn;
        case MAL_TIMER_14:
        default:
            return TIM8_TRG_COM_TIM14_IRQn;
    }
}

MAL_DEFS_INLINE bool mal_timer_disable_interrupt(mal_timer_s *handle) {
    bool active = (bool)NVIC_GetEnableIRQ(handle->irq);
    NVIC_DisableIRQ(handle->irq);
    __DSB();
    __ISB();
    return active;
}

MAL_DEFS_INLINE void mal_timer_enable_interrupt(mal_timer_s *handle, bool active) {
    if (active) {
        NVIC_EnableIRQ(handle->irq);
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

static mal_error_e mal_hspec_stm32f7_timer_common_init(mal_timer_e timer, mal_timer_s *handle) {
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
    // Save timer
    handle->timer = timer;

    return MAL_ERROR_OK;
}

static bool mal_hspec_stm32f7_timer_is_used(mal_timer_e timer) {
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
    if (NULL != local_handle) {
        return true;
    }
    return false;
}

static mal_error_e mal_hspec_stm32f7_timer_common_update_init(mal_timer_e timer, mal_timer_s *handle,
                                                              mal_hertz_t frequency, mal_hertz_t delta) {
    mal_error_e mal_result;
    // Check if timer is already initialized
    bool validate_parameters = mal_hspec_stm32f7_timer_is_used(timer);
    // Execute common initialization
    mal_result = mal_hspec_stm32f7_timer_common_init(timer, handle);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
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
    }

    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f7_timer_common_count_init(mal_timer_e timer, mal_timer_s *handle,
                                                             mal_hertz_t frequency) {
    mal_error_e mal_result;
    // Check if timer is already initialized
    bool validate_parameters = mal_hspec_stm32f7_timer_is_used(timer);
    // Execute common initialization
    mal_result = mal_hspec_stm32f7_timer_common_init(timer, handle);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Get timer input clock
    uint64_t timer_frequency;
    mal_result = mal_hspec_stm32f7_timer_get_input_clk(timer, &timer_frequency);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Try to find proper settings for requested frequency
    uint64_t target_frequency = MAL_TYPES_MAL_HERTZ_TO_MILLIHERTZ(frequency);
    uint32_t prescaler = 0;
    bool found = false;
    for (prescaler = 1; prescaler <= (UINT16_MAX + 1); prescaler++) {
        uint64_t potential_frequency = timer_frequency / (uint64_t)prescaler;
        if (potential_frequency != target_frequency) {
            continue;
        }
        found = true;
        break;
    }
    if (!found) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    // Fetch mask to get max period
    uint64_t mask;
    mal_result = mal_timer_get_count_mask(timer, &mask);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Remove one because register is 0 based.
    prescaler -= 1;
    // Validate or set
    if (validate_parameters) {
        if ((handle->hal_timer_handle.Init.Prescaler != prescaler) ||
            (handle->hal_timer_handle.Init.Period != (uint32_t)mask)) {
            return MAL_ERROR_HARDWARE_UNAVAILABLE;
        }
        return MAL_ERROR_ALREADY_INITIALIZED;
    } else {
        handle->hal_timer_handle.Init.Prescaler = prescaler;
        handle->hal_timer_handle.Init.Period = (uint32_t)mask;
        handle->hal_timer_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
        handle->hal_timer_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        handle->hal_timer_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        handle->hal_timer_handle.Init.RepetitionCounter = 0;
        handle->hal_timer_handle.Parent = handle;
    }

    return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_task(mal_timer_init_task_s *init, mal_timer_s *handle) {
    mal_error_e mal_result;
    HAL_StatusTypeDef hal_result;
    // Common initialisation such as clock, local handles and more.
    mal_result = mal_hspec_stm32f7_timer_common_update_init(init->timer, handle, init->frequency, init->delta);
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
    handle->irq = mal_hspec_stm32f7_timer_get_update_irq(init->timer);
    NVIC_EnableIRQ(handle->irq);
    NVIC_SetPriority(handle->irq, 2); // Find a way to manage priorities for interrupts
    // Enable timer interrupt
    handle->task_callback = init->callback;
    handle->callback_handle = init->callback_handle;
    HAL_TIM_Base_Start_IT(&handle->hal_timer_handle);
}

static mal_error_e mal_hspec_stm32f7_timer_get_channel(mal_timer_e timer, mal_gpio_port_e port, uint8_t pin,
                                                       uint32_t *channel) {
    switch (timer) {
        case MAL_TIMER_1:
            if ((MAL_GPIO_PORT_A == port && 7 == pin) ||
                (MAL_GPIO_PORT_E == port && 8 == pin) ||
                (MAL_GPIO_PORT_E == port && 9 == pin) ||
                (MAL_GPIO_PORT_B == port && 13 == pin) ||
                (MAL_GPIO_PORT_A == port && 8 == pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == port && 0 == pin) ||
                       (MAL_GPIO_PORT_E == port && 10 == pin) ||
                       (MAL_GPIO_PORT_E == port && 11 == pin) ||
                       (MAL_GPIO_PORT_B == port && 14 == pin) ||
                       (MAL_GPIO_PORT_A == port && 9 == pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == port && 1 == pin) ||
                       (MAL_GPIO_PORT_E == port && 12 == pin) ||
                       (MAL_GPIO_PORT_E == port && 13 == pin) ||
                       (MAL_GPIO_PORT_B == port && 15 == pin) ||
                       (MAL_GPIO_PORT_A == port && 10 == pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_E == port && 14 == pin) ||
                       (MAL_GPIO_PORT_A == port && 11 == pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_2:
            if ((MAL_GPIO_PORT_A == port && 0 == pin) ||
                (MAL_GPIO_PORT_A == port && 5 == pin) ||
                (MAL_GPIO_PORT_A == port && 15 == pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == port && 1 == pin) ||
                       (MAL_GPIO_PORT_B == port && 3 == pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == port && 2 == pin) ||
                       (MAL_GPIO_PORT_B == port && 10 == pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == port && 3 == pin) ||
                       (MAL_GPIO_PORT_B == port && 11 == pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_3:
            if ((MAL_GPIO_PORT_A == port && 6 == pin) ||
                (MAL_GPIO_PORT_C == port && 6 == pin) ||
                (MAL_GPIO_PORT_B == port && 4 == pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == port && 7 == pin) ||
                       (MAL_GPIO_PORT_C == port && 7 == pin) ||
                       (MAL_GPIO_PORT_B == port && 5 == pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == port && 0 == pin) ||
                       (MAL_GPIO_PORT_C == port && 8 == pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == port && 1 == pin) ||
                       (MAL_GPIO_PORT_C == port && 9 == pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_4:
            if ((MAL_GPIO_PORT_D == port && 12 == pin) ||
                (MAL_GPIO_PORT_B == port && 6 == pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_D == port && 13 == pin) ||
                       (MAL_GPIO_PORT_B == port && 7 == pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_D == port && 14 == pin)  ||
                       (MAL_GPIO_PORT_B == port && 8 == pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_D == port && 15 == pin) ||
                       (MAL_GPIO_PORT_B == port && 9 == pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_5:
            if ((MAL_GPIO_PORT_A == port && 0 == pin) ||
                (MAL_GPIO_PORT_H == port && 10 == pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == port && 1 == pin) ||
                       (MAL_GPIO_PORT_H == port && 11 == pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == port && 2 == pin) ||
                       (MAL_GPIO_PORT_H == port && 12 == pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_A == port && 3 == pin) ||
                       (MAL_GPIO_PORT_I == port && 0 == pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_8:
            if ((MAL_GPIO_PORT_A == port && 5 == pin) ||
                (MAL_GPIO_PORT_A == port && 7 == pin) ||
                (MAL_GPIO_PORT_C == port && 6 == pin) ||
                (MAL_GPIO_PORT_H == port && 13 == pin) ||
                (MAL_GPIO_PORT_I == port && 5 == pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == port && 0 == pin) ||
                       (MAL_GPIO_PORT_B == port && 14 == pin) ||
                       (MAL_GPIO_PORT_C == port && 7 == pin) ||
                       (MAL_GPIO_PORT_H == port && 14 == pin) ||
                       (MAL_GPIO_PORT_I == port && 6 == pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_B == port && 1 == pin) ||
                       (MAL_GPIO_PORT_B == port && 15 == pin) ||
                       (MAL_GPIO_PORT_C == port && 8 == pin) ||
                       (MAL_GPIO_PORT_H == port && 15 == pin) ||
                       (MAL_GPIO_PORT_I == port && 7 == pin)) {
                *channel = TIM_CHANNEL_3;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_C == port && 9 == pin) ||
                       (MAL_GPIO_PORT_I == port && 2 == pin)) {
                *channel = TIM_CHANNEL_4;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_9:
            if ((MAL_GPIO_PORT_E == port && 5 == pin) ||
                (MAL_GPIO_PORT_A == port && 2 == pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_E == port && 6 == pin) ||
                       (MAL_GPIO_PORT_A == port && 3 == pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_10:
            if ((MAL_GPIO_PORT_F == port && 6 == pin) ||
                (MAL_GPIO_PORT_B == port && 8 == pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_11:
            if ((MAL_GPIO_PORT_F == port && 7 == pin) ||
                (MAL_GPIO_PORT_B == port && 9 == pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_12:
            if ((MAL_GPIO_PORT_H == port && 6 == pin) ||
                (MAL_GPIO_PORT_B == port && 14 == pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else if ((MAL_GPIO_PORT_H == port && 9 == pin) ||
                       (MAL_GPIO_PORT_B == port && 15 == pin)) {
                *channel = TIM_CHANNEL_2;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_13:
            if ((MAL_GPIO_PORT_F == port && 8 == pin) ||
                (MAL_GPIO_PORT_A == port && 6 == pin)) {
                *channel = TIM_CHANNEL_1;
                return MAL_ERROR_OK;
            } else {
                return MAL_ERROR_HARDWARE_INVALID;
            }
        case MAL_TIMER_14:
            if ((MAL_GPIO_PORT_F == port && 9 == pin) ||
                (MAL_GPIO_PORT_A == port && 7 == pin)) {
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
    common_result = mal_hspec_stm32f7_timer_common_update_init(init->timer, handle, init->frequency, init->delta);
    if (MAL_ERROR_OK != common_result && MAL_ERROR_ALREADY_INITIALIZED != common_result) {
        return common_result;
    }
    // Initialize PWM IO
    mal_result = mal_hspec_stm32f7_timer_init_io(init->timer, init->port, init->pin);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
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
    mal_result = mal_hspec_stm32f7_timer_get_channel(init->timer, init->port, init->pin, &pwm_handle->channel);
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

mal_error_e mal_timer_init_count(mal_timer_init_count_s *init, mal_timer_s *handle) {
    HAL_StatusTypeDef hal_result;
    // Execute common init
    mal_error_e common_result;
    common_result = mal_hspec_stm32f7_timer_common_count_init(init->timer, handle, init->frequency);
    if (MAL_ERROR_OK != common_result && MAL_ERROR_ALREADY_INITIALIZED != common_result) {
        return common_result;
    }
    // Initialise timer
    if (MAL_ERROR_ALREADY_INITIALIZED == common_result) {
        if (MAL_HSPEC_STM32F7_TIMER_MODE_COUNT != handle->mode) {
            return MAL_ERROR_HARDWARE_UNAVAILABLE;
        }
    } else {
        hal_result = HAL_TIM_Base_Init(&handle->hal_timer_handle);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_HARDWARE_INVALID;
        }
        handle->mode = MAL_HSPEC_STM32F7_TIMER_MODE_COUNT;
    }
    hal_result = HAL_TIM_Base_Start(&handle->hal_timer_handle);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    return MAL_ERROR_OK;
}

static mal_error_e mal_hspec_stm32f7_timer_init_io(mal_timer_e timer, mal_gpio_port_e port, uint8_t pin) {
    mal_error_e mal_result;
    // Enable input IO clock
    mal_result = mal_hspec_stm32f7_gpio_enable_clock(port);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Set IO in input capture mode
    GPIO_TypeDef *hal_port = mal_hspec_stm32f7_gpio_get_hal_port(port);
    GPIO_InitTypeDef gpio_init;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init.Pin = MAL_HSPEC_STM32F7_GPIO_GET_HAL_PIN(pin);
    mal_result = mal_hspec_stm32f7_timer_get_alternate(timer, &gpio_init.Alternate);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    HAL_GPIO_Init(hal_port, &gpio_init);

    return MAL_ERROR_OK;
}

mal_error_e mal_timer_init_input_capture(mal_timer_init_intput_capture_s *init, mal_timer_s *handle,
                                         mal_timer_input_capture_s *input_capture_handle) {
    mal_error_e mal_result;
    HAL_StatusTypeDef hal_result;
    // Common initialisation such as clock, local handles and more.
    mal_error_e common_result;
    common_result = mal_hspec_stm32f7_timer_common_count_init(init->timer, handle, init->frequency);
    if (MAL_ERROR_OK != common_result && MAL_ERROR_ALREADY_INITIALIZED != common_result) {
        return common_result;
    }
    // Initialize input capture IO
    mal_result = mal_hspec_stm32f7_timer_init_io(init->timer, init->port, init->pin);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    // Initialise timer in input capture mode
    if (MAL_ERROR_ALREADY_INITIALIZED == common_result) {
        if (MAL_HSPEC_STM32F7_TIMER_MODE_COUNT != handle->mode) {
            return MAL_ERROR_HARDWARE_UNAVAILABLE;
        }
    } else {
        hal_result = HAL_TIM_IC_Init(&handle->hal_timer_handle);
        if (HAL_OK != hal_result) {
            return MAL_ERROR_HARDWARE_INVALID;
        }
        handle->mode = MAL_HSPEC_STM32F7_TIMER_MODE_COUNT;
    }
    // Get channel index
    uint32_t channel;
    mal_result = mal_hspec_stm32f7_timer_get_channel(init->timer, init->port, init->pin, &channel);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    uint32_t channel_index = channel >> 2U;
    // Set input capture handle
    handle->input_capture_handles[channel_index] = input_capture_handle;
    input_capture_handle->handle = handle;
    input_capture_handle->callback = init->callback;
    input_capture_handle->callback_handle = init->callback_handle;
    // Configure channel
    TIM_IC_InitTypeDef ic_init;
    if (MAL_TIMER_INPUT_RISING == init->input_event) {
        ic_init.ICPolarity = TIM_ICPOLARITY_RISING;
    } else if (MAL_TIMER_INPUT_FALLING == init->input_event) {
        ic_init.ICPolarity = TIM_ICPOLARITY_FALLING;
    } else {
        ic_init.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;
    }
    ic_init.ICSelection = TIM_ICSELECTION_DIRECTTI;
    switch (init->input_divider) {
        case 1:
            ic_init.ICPrescaler = TIM_ICPSC_DIV1;
            break;
        case 2:
            ic_init.ICPrescaler = TIM_ICPSC_DIV2;
            break;
        case 4:
            ic_init.ICPrescaler = TIM_ICPSC_DIV4;
            break;
        case 8:
            ic_init.ICPrescaler = TIM_ICPSC_DIV8;
            break;
        default:
            return MAL_ERROR_HARDWARE_INVALID;
    }
    ic_init.ICFilter = 0;
    hal_result = HAL_TIM_IC_ConfigChannel(&handle->hal_timer_handle, &ic_init, channel);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    // Enable NVIC interrupt for timer
    handle->irq = mal_hspec_stm32f7_timer_get_compare_irq(init->timer);
    NVIC_EnableIRQ(handle->irq);
    NVIC_SetPriority(handle->irq, 2); // Find a way to manage priorities for interrupts
    // Start timer
    HAL_TIM_IC_Start_IT(&handle->hal_timer_handle, channel);

    return MAL_ERROR_OK;
}

mal_error_e mal_timer_get_count(mal_timer_s *handle, uint64_t *count) {
    *count = handle->hal_timer_handle.Instance->CNT;
    return MAL_ERROR_OK;
}

mal_error_e mal_timer_free(mal_timer_s *handle) {
    HAL_StatusTypeDef hal_result;
    // Make sure timer is used
    if (!mal_hspec_stm32f7_timer_is_used(handle->timer)) {
        return MAL_ERROR_OPERATION_INVALID;
    }
    // Disable interrupt
    mal_timer_disable_interrupt(handle);
    // Disable timer
    hal_result = HAL_TIM_Base_DeInit(&handle->hal_timer_handle);
    if (HAL_OK != hal_result) {
        return MAL_ERROR_HARDWARE_INVALID;
    }
    // Reset peripheral
    switch (handle->timer) {
        case MAL_TIMER_1:
            __HAL_RCC_TIM1_FORCE_RESET();
            timer1_handle = NULL;
            break;
        case MAL_TIMER_2:
            __HAL_RCC_TIM2_FORCE_RESET();
            timer2_handle = NULL;
            break;
        case MAL_TIMER_3:
            __HAL_RCC_TIM3_FORCE_RESET();
            timer3_handle = NULL;
            break;
        case MAL_TIMER_4:
            __HAL_RCC_TIM4_FORCE_RESET();
            timer4_handle = NULL;
            break;
        case MAL_TIMER_5:
            __HAL_RCC_TIM5_FORCE_RESET();
            timer5_handle = NULL;
            break;
        case MAL_TIMER_6:
            __HAL_RCC_TIM6_FORCE_RESET();
            timer6_handle = NULL;
            break;
        case MAL_TIMER_7:
            __HAL_RCC_TIM7_FORCE_RESET();
            timer7_handle = NULL;
            break;
        case MAL_TIMER_8:
            __HAL_RCC_TIM8_FORCE_RESET();
            timer8_handle = NULL;
            break;
        case MAL_TIMER_9:
            __HAL_RCC_TIM9_FORCE_RESET();
            timer9_handle = NULL;
            break;
        case MAL_TIMER_10:
            __HAL_RCC_TIM10_FORCE_RESET();
            timer10_handle = NULL;
            break;
        case MAL_TIMER_11:
            __HAL_RCC_TIM11_FORCE_RESET();
            timer11_handle = NULL;
            break;
        case MAL_TIMER_12:
            __HAL_RCC_TIM12_FORCE_RESET();
            timer12_handle = NULL;
            break;
        case MAL_TIMER_13:
            __HAL_RCC_TIM13_FORCE_RESET();
            timer13_handle = NULL;
            break;
        case MAL_TIMER_14:
            __HAL_RCC_TIM14_FORCE_RESET();
            timer14_handle = NULL;
            break;
        default:
            return MAL_ERROR_OPERATION_INVALID;
    }
    // Flush handles
    handle->input_capture_handles[0] = NULL;
    handle->input_capture_handles[1] = NULL;
    handle->input_capture_handles[2] = NULL;
    handle->input_capture_handles[3] = NULL;
    handle->callback_handle = NULL;

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
    mal_timer_s *handle = htim->Parent;
    handle->task_callback(handle->callback_handle);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    // Get input capture handle
    mal_timer_s *handle = htim->Parent;
    mal_timer_input_capture_s *input_capture_handle;
    switch (htim->Channel) {
        case HAL_TIM_ACTIVE_CHANNEL_1:
            input_capture_handle = handle->input_capture_handles[0];
            break;
        case HAL_TIM_ACTIVE_CHANNEL_2:
            input_capture_handle = handle->input_capture_handles[1];
            break;
        case HAL_TIM_ACTIVE_CHANNEL_3:
            input_capture_handle = handle->input_capture_handles[2];
            break;
        case HAL_TIM_ACTIVE_CHANNEL_4:
            input_capture_handle = handle->input_capture_handles[3];
            break;
        default:
            // Should not get here
            return;
    }
    if (NULL == input_capture_handle) {
        return;
    }
    // Execute callback
    input_capture_handle->callback(input_capture_handle->callback_handle, handle->hal_timer_handle.Instance->CNT);
}
