/*
 * mal_hspec_stm32f0_can.h
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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_CAN_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_CAN_H_

#include "hspec/mal_hspec.h"
#include "stm32f0/stm32f0xx_misc.h"
#include "std/mal_error.h"

// Enable interrupts
// 30 equates to CAN_IRQ. However, the name of the constant changes based
// on the MCU because it is not available on all of them. It is simpler to
// use the constant directly.
#define mal_hspec_stm32f0_disable_can_interrupt(interface) do {\
	NVIC_DisableIRQ(30); \
	__DSB(); \
	__ISB(); \
} while(0)

#define mal_hspec_stm32f0_enable_can_interrupt(interface) do {\
	NVIC_EnableIRQ(30); \
} while(0)

mal_error_e mal_hspec_stm32f0_can_init(mal_hspec_can_init_s *init);

mal_error_e mal_hspec_stm32f0_can_transmit(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg);

mal_error_e mal_hspec_stm32f0_can_add_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter);

mal_error_e mal_hspec_stm32f0_can_remove_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_CAN_H_ */
