/*
 * mal_hspec_stm32f072_flash.h
 *
 *  Created on: Sep 7, 2016
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

#ifndef HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_FLASH_H_
#define HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_FLASH_H_

#include "std/mal_stdint.h"

uint32_t mal_hspec_stm32f072_flash_get_page_size(uint32_t page);

#endif /* HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_FLASH_H_ */