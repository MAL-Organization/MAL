/*
 * mal_hspec_stm32f0_gpio.h
 *
 *  Created on: Jun 24, 2015
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_GPIO_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_GPIO_H_

#include "std/mal_error.h"
#include "std/mal_bool.h"

#define mal_hspec_stm32f0_gpio_event_disable_interrupt(gpio) do {\
	NVIC_DisableIRQ(mal_hspec_stm32f0_gpio_get_exti_irq((gpio)->pin)); \
	__DSB(); \
	__ISB(); \
} while(0)

#define mal_hspec_stm32f0_gpio_event_enable_interrupt(gpio) do {\
	NVIC_EnableIRQ(mal_hspec_stm32f0_gpio_get_exti_irq((gpio)->pin)); \
} while(0)

/**
 * Specific implementation of setting direction of gpio for STM.
 * \param gpio [in] The desired GPIO.
 * \param dir  [in] The desired direction.
 * \return Returns an error code.
 */
mal_error_e mal_hspec_stm32f0_gpio_init(mal_hspec_gpio_init_s *gpio_init);

mal_error_e mal_hspec_stm32f0_set_gpio(const mal_hspec_gpio_s *gpio, bool value);

mal_error_e mal_hspec_stm32f0_toggle_gpio(const mal_hspec_gpio_s *gpio);

bool mal_hspec_stm32f0_get_gpio(const mal_hspec_gpio_s *gpio);

mal_error_e mal_hspec_stm32f0_gpio_event_init(mal_hspec_gpio_event_init_s *init);

mal_error_e mal_hspec_stm32f0_gpio_event_remove(const mal_hspec_gpio_s *gpio);

IRQn_Type mal_hspec_stm32f0_gpio_get_exti_irq(uint8_t pin);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_GPIO_H_ */
