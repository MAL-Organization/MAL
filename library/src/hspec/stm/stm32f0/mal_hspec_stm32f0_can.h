/*
 * Copyright (c) 2018 Olivier Allaire
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

#include "can/mal_can.h"
#include "std/mal_stdlib.h"
#include "std/mal_stdint.h"
#include "std/mal_bool.h"
#include "hspec/stm/mal_hspec_stm_bcan.h"

typedef struct MAL_CAN {
    mal_can_tx_callback_t tx_callback;
    void *tx_callback_handle;
    mal_can_rx_callback_t rx_callback;
    void *rx_callback_handle;
    mal_hspec_stm_bcan_filter_banks_s can_filter_banks;
    volatile bool interface_active;
} mal_can_s;

typedef struct MAL_CAN_INTERRUPT_STATE {
    bool active;
} mal_can_interrupt_state_s;

/**
 * Struct for direct initialization of a CAN interface.
 */
typedef struct {
	uint8_t bs1;		//!< TSEG1 register
	uint8_t bs2;		//!< TSEG2 register
	uint8_t sjw;		//!< SJW register
	uint16_t prescaler;	//!< Prescaler register
} mal_hspec_stm32f0_can_direct_init_s;

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_CAN_H_ */
