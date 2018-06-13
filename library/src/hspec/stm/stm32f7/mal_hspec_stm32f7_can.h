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

#ifndef MAL_MAL_HSPEC_STM32F7_CAN_H
#define MAL_MAL_HSPEC_STM32F7_CAN_H

#include "stm32f7/stm32f7xx_hal_can.h"
#include "stm32f7/stm32f7xx.h"
#include "hspec/stm/mal_hspec_stm_bcan.h"
#include "std/mal_bool.h"
#include "can/mal_can.h"

typedef struct MAL_CAN {
    CAN_HandleTypeDef hal_can_handle;
    mal_can_tx_callback_t tx_callback;
    void *tx_callback_handle;
    mal_can_rx_callback_t rx_callback;
    void *rx_callback_handle;
    bool interface_active;
    IRQn_Type f0_irq;
    IRQn_Type f1_irq;
    IRQn_Type tx_irq;
    mal_hspec_stm_bcan_filter_banks_s filter_banks;
} mal_can_s;

typedef struct MAL_CAN_INTERRUPT_STATE {
    bool f0_irq_state;
    bool f1_irq_state;
    bool tx_irq_state;
} mal_can_interrupt_state_s;

#endif //MAL_MAL_HSPEC_STM32F7_CAN_H
