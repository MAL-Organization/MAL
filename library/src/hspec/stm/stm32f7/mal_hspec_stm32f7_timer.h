/*
 * mal_hspec_stm32f7_timer.h
 *
 *  Created on: May 8, 2018
 *      Author: olivi
 */

#ifndef HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_TIMER_H_
#define HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_TIMER_H_

#include "timer/mal_timer2.h"
#include "stm32f7/stm32f7xx.h"
#include "stm32f7/stm32f7xx_hal_tim.h"

typedef struct MAL_TIMER {
    IRQn_Type update_irq;
    TIM_HandleTypeDef hal_timer_handle;
} mal_timer_s;

#endif /* HSPEC_STM_STM32F7_MAL_HSPEC_STM32F7_TIMER_H_ */
