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

#ifndef HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_I2C_H_
#define HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_I2C_H_

#include "i2c/mal_i2c.h"
#include "std/mal_stdint.h"
#include "std/mal_bool.h"
#include "stm32f0/stm32f0xx_i2c.h"

typedef enum {
    I2C_STATE_START,
    I2C_STATE_RECEIVING,
    I2C_STATE_ERROR,
    I2C_STATE_WAITING_TRANSFER_COMPLETE,
    I2C_STATE_TRANSMITTING,
    I2C_STATE_WAIT_STOP
} mal_hspec_stm32f0_i2c_states_e;

typedef struct MAL_I2C {
    mal_i2c_msg_s *msg;
    volatile bool is_active;
    volatile mal_hspec_stm32f0_i2c_states_e state;
    volatile uint8_t data_ptr;
    volatile mal_i2c_cmd_e cmd;
    I2C_TypeDef *stm_handle;
    IRQn_Type irq;
} mal_i2c_s;

typedef struct MAL_I2C_INTERRUPT_STATE {
    bool active;
} mal_i2c_interrupt_state_s;

/**
 * Struct for direct initialization of a CAN interface.
 */
typedef struct {
	uint32_t timing;	//!< Timing parameter from I2C_InitTypeDef.
} mal_hspec_stm32f0_i2c_direct_init_s;

#endif /* HSPEC_STM_STM32F0_MAL_HSPEC_STM32F0_I2C_H_ */
