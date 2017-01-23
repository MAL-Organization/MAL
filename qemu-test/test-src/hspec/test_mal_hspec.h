/*
 * test_mal_hspec.h
 *
 *  Created on: Jan 23, 2017
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

#ifndef HSPEC_TEST_MAL_HSPEC_H_
#define HSPEC_TEST_MAL_HSPEC_H_

// Family specific function mapping
#ifdef MAL_STM32F0
#include "hspec/stm/stm32f0/test_mal_hspec_stm32f0.h"

// TIMER

#define test_mal_hspec_init_tick_1khz(timer) test_mal_hspec_stm32f0_init_tick_1khz(timer)

#else
#error No hardware specfic family symbol specified...
#endif

#endif /* HSPEC_TEST_MAL_HSPEC_H_ */
