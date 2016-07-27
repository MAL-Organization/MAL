/*
 * mal_hspec_stm32f0_reset.h
 *
 *  Created on: Jul 27, 2016
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_RESET_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_RESET_H_

#include "hspec/mal_hspec_cmn.h"

mal_hspec_reset_source_e mal_hspec_stm32f0_reset_handle_reset_source(void);

void mal_hspec_stm32f0_reset_mcu(void);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_RESET_H_ */
