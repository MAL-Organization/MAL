/*
 * mal_hspec_stm32f030r8.h
 *
 *  Created on: May 3, 2015
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

#ifndef HSPEC_STM_MAL_HSPEC_STM32F030R8_H_
#define HSPEC_STM_MAL_HSPEC_STM32F030R8_H_

#include "std/mal_stdint.h"
#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"

mal_error_e mal_hspec_stm32f030r8_get_valid_ports(const mal_hspec_port_e **ports, uint8_t *size);
mal_error_e mal_hspec_stm32f030r8_get_valid_pins(const uint64_t **pins);

#endif /* HSPEC_STM_MAL_HSPEC_STM32F030R8_H_ */
