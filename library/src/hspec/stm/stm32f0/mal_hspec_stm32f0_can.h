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
#include "std/mal_bool.h"
#include "std/mal_stdint.h"

/**
 * Struct for direct initialization of a CAN interface.
 */
typedef struct {
	uint8_t bs1;		//!< TSEG1 register
	uint8_t bs2;		//!< TSEG2 register
	uint8_t sjw;		//!< SJW register
	uint16_t prescaler;	//!< Prescaler register
} mal_hspec_stm32f0_can_direct_init_s;

#define mal_hspec_stm32f0_enable_can_interrupt(interface, active) do {\
	if (active) { \
		NVIC_EnableIRQ(30); \
	} \
} while(0)

mal_error_e mal_hspec_stm32f0_can_init(mal_hspec_can_init_s *init);

mal_error_e mal_hspec_stm32f0_can_direct_init(mal_hspec_can_init_s *init, const void *direct_init);

mal_error_e mal_hspec_stm32f0_can_transmit(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg);

mal_error_e mal_hspec_stm32f0_can_add_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter);

mal_error_e mal_hspec_stm32f0_can_remove_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter);

bool mal_hspec_stm32f0_disable_can_interrupt(mal_hspec_can_e interface);

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_CAN_H_ */
