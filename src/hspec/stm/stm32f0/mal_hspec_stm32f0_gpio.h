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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_GPIO_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_GPIO_H_

#include "std/mal_error.h"
#include "std/mal_bool.h"

/**
 * Specific implementation of setting direction of gpio for STM.
 * \param gpio [in] The desired GPIO.
 * \param dir  [in] The desired direction.
 * \return Returns an error code.
 */
mal_error_e mal_hspec_stm32f0_gpio_init(mal_hpsec_gpio_init_s *gpio_init);

mal_error_e mal_hspec_stm32f0_set_gpio(mal_hspec_gpio_s *gpio, bool value);

mal_error_e mal_hspec_stm32f0_toggle_gpio(mal_hspec_gpio_s *gpio);

bool mal_hspec_stm32f0_get_gpio(mal_hspec_gpio_s *gpio);


#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_GPIO_H_ */
