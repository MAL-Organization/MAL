/*
 * mal_hspec_stm32f0_nvic.c
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

#include "mal_hspec_stm32f0_nvic.h"
#include "cm0/core_cm0.h"

uint32_t mal_hspec_stm32f0_nvic_add_irq(IRQn_Type irq, uint32_t mask) {
    return mask | (1 << irq);
}

uint32_t mal_hspec_stm32f0_nvic_remove_irq(IRQn_Type irq, uint32_t mask) {
    return mask & ~(1 << irq);
}

uint32_t mal_hspec_stm32f0_nvic_get_activity(uint32_t mask) {
    return nvic_status & mask;
}

void mal_hspec_stm32f0_nvic_set(uint32_t mask) {
    NVIC->ISER[0] = mask;
    nvic_status |= mask;
}

void mal_hspec_stm32f0_nvic_clear(uint32_t mask) {
    NVIC->ICER[0] = mask;
    nvic_status &= ~mask;
}
