/*
 * mal_hspec_stm32f072.h
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

#ifndef HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_H_
#define HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_H_

#include "hspec/stm/stm32f0/stm32f072/mal_hspec_stm32f072_flash.h"
#include "hspec/stm/stm32f0/stm32f072/mal_hspec_stm32f072_timer.h"
#include "hspec/stm/stm32f0/stm32f072/mal_hspec_stm32f072_dac.h"

#include "hspec/mal_hspec_cmn.h"
#include "std/mal_bool.h"

bool mal_hspec_stm32f072_is_pll_div_available(mal_hspec_system_clk_src_e source);

#endif /* HSPEC_STM_STM32F0_STM32F072_MAL_HSPEC_STM32F072_H_ */
