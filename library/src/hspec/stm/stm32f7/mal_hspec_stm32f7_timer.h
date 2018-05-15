/*
 * mal_hspec_stm32f7_timer.h
 *
 *  Created on: May 8, 2018
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

#ifndef HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_TIMER_H_
#define HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_TIMER_H_

#include "timer/mal_timer2.h"
#include "stm32f7/stm32f7xx.h"
#include "stm32f7/stm32f7xx_hal_tim.h"

typedef enum {
    MAL_HSPEC_STM32F7_TIMER_MODE_BASIC,
    MAL_HSPEC_STM32F7_TIMER_MODE_PWM
} mal_hspec_stm32f7_timer_mode_e;

typedef struct MAL_TIMER {
    IRQn_Type update_irq;
    TIM_HandleTypeDef hal_timer_handle;
    mal_timer_callback_t task_callback;
    void *callback_handle;
    mal_hspec_stm32f7_timer_mode_e  mode;
} mal_timer_s;

typedef struct MAL_TIMER_PWM {
    mal_timer_s *handle;
    uint32_t channel;
} mal_timer_pwm_s;

#endif /* HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_TIMER_H_ */
