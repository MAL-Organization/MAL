/*
 * mal_hspec_stm32f0_nvic.h
 *
 *  Created on: Mar 7, 2018
 *      Author: Olivier
 */
/*
 * Copyright (c) 2017 Olivier Allaire
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_NVIC_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_NVIC_H_

#include "std/mal_stdint.h"
#include "stm32f0/stm32f0xx.h"

uint32_t mal_hspec_stm32f0_nvic_add_irq(IRQn_Type irq, uint32_t mask);

uint32_t mal_hspec_stm32f0_nvic_remove_irq(IRQn_Type irq, uint32_t mask);

uint32_t mal_hspec_stm32f0_nvic_get_activity(uint32_t mask);

void mal_hspec_stm32f0_nvic_set(uint32_t mask);

void mal_hspec_stm32f0_nvic_clear(uint32_t mask);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_NVIC_H_ */
