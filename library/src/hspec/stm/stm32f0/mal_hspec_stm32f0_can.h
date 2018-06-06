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

#define MAL_HSPEC_STM32F0_CAN_FILTER_BANKS_SIZE	14
#define MAL_HSPEC_STM32F0_CAN_FILTER_STD_SIZE		2

typedef struct {
    uint32_t id;
    uint32_t mask;
} mal_hspec_stm32f0_can_extended_filter_s;

typedef struct {
    uint16_t id[MAL_HSPEC_STM32F0_CAN_FILTER_STD_SIZE];
    uint16_t mask[MAL_HSPEC_STM32F0_CAN_FILTER_STD_SIZE];
} mal_hspec_stm32f0_can_standard_filter_s;

typedef union {
    mal_hspec_stm32f0_can_extended_filter_s ext;
    mal_hspec_stm32f0_can_standard_filter_s std;
} mal_hspec_stm32f0_can_filter_u;

typedef struct {
    mal_hspec_stm32f0_can_filter_u filter;
    uint8_t filter_count;
    mal_can_id_type_e type;
    bool is_active;
    uint8_t fifo;
} mal_hspec_stm32f0_can_filter_bank_s;

typedef struct MAL_CAN {
    mal_can_tx_callback_t tx_callback;
    void *tx_callback_handle;
    mal_can_rx_callback_t rx_callback;
    void *rx_callback_handle;
    mal_hspec_stm32f0_can_filter_bank_s can_filter_banks[MAL_HSPEC_STM32F0_CAN_FILTER_BANKS_SIZE];
    volatile bool interface_active;
} mal_can_s;

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
