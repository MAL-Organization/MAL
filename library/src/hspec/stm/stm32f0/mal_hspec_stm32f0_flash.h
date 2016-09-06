/*
 * mal_hspec_stm32f0_flash.h
 *
 *  Created on: Sep 2, 2016
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_FLASH_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_FLASH_H_

#include "std/mal_error.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_stm32f0_flash_erase_page(uint32_t page);

mal_error_e mal_hspec_stm32f0_flash_write_uint16_values(uint64_t start_address, uint16_t *values, uint32_t count);

mal_error_e mal_hspec_stm32f0_flash_write_uint32_values(uint64_t start_address, uint32_t *values, uint32_t count);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_FLASH_H_ */
