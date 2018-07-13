/*
 * mal_hspec_stm32f0_can.c
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

#include "mal_hspec_stm32f0_can.h"
#include "std/mal_stdlib.h"
#include "std/mal_math.h"
#include "can/mal_can.h"
#include "mal_hspec_stm32f0_cmn.h"
#include "cm0/core_cm0.h"
#include "std/mal_defs.h"

static mal_error_e mal_hspec_stm32f0_can_init_common(mal_can_init_s *init, mal_can_s *handle);

static void can_read_fifo(uint8_t fifo);

static void can_transmit_msg(mal_can_msg_s *msg);

static mal_can_s *can_handle;

static mal_error_e mal_hspec_stm32f0_can_init_common(mal_can_init_s *init, mal_can_s *handle) {
	// Enable GPIO clocks
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->tx_port), ENABLE);
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->rx_port), ENABLE);
	// Configure alternate function
	mal_error_e result;
	uint8_t function;
	result = mal_hspec_stm32f0_get_pin_af(init->tx_port, init->tx_pin, MAL_HSPEC_STM32F0_AF_CAN_TX, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->tx_port), init->tx_pin, function);

	result = mal_hspec_stm32f0_get_pin_af(init->rx_port, init->rx_pin, MAL_HSPEC_STM32F0_AF_CAN_RX, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->rx_port), init->rx_pin, function);
	// Configure GPIOs
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->tx_pin);
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->tx_port), &gpio_init);

	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->rx_pin);
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->rx_port), &gpio_init);
	// Enable CAN clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);
	// Clear CAN
	CAN_DeInit(CAN);
	// Initialize handle
    mal_hspec_stm_bcan_init_filter_banks(&handle->can_filter_banks);
    handle->interface_active = false;
    can_handle = handle;

	return MAL_ERROR_OK;
}

mal_error_e mal_can_init(mal_can_init_s *init, mal_can_s *handle) {
	mal_error_e result;
	// Initialize common part of CAN
	result = mal_hspec_stm32f0_can_init_common(init, handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Get APB Clock
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	// Compute bit time
	uint32_t prescaler;
	uint32_t tseg1 = 0;
	int32_t tseg2 = 0;
	uint32_t sjw = 0;
	result = mal_hspec_stm_bcan_get_bit_timing(init->bitrate, clocks.PCLK_Frequency, &prescaler, &tseg1, &tseg2, &sjw);
    if (MAL_ERROR_OK != result) {
        return result;
    }
	// Save call backs
	handle->tx_callback = init->tx_callback;
	handle->tx_callback_handle = init->tx_callback_handle;
	handle->rx_callback = init->rx_callback;
	handle->rx_callback_handle = init->rx_callback_handle;
	// Configure CAN
	CAN_InitTypeDef can_init;
	CAN_StructInit(&can_init);
	can_init.CAN_BS1 = (uint8_t)tseg1;
	can_init.CAN_BS2 = (uint8_t)tseg2;
	can_init.CAN_SJW = (uint8_t)sjw;
	can_init.CAN_Prescaler = (uint16_t)prescaler;
	can_init.CAN_ABOM = ENABLE;
	if (CAN_InitStatus_Success != CAN_Init(CAN, &can_init)) {
		return MAL_ERROR_INIT_FAILED;
	}
	// Enable interrupts
	// 30 equates to CAN_IRQ. However, the name of the constant changes based
	// on the MCU because it is not available on all of them. It is simpler to
	// use the constant directly. If the MCU does not support CAN, the code
	// will not get here.
	NVIC_EnableIRQ((IRQn_Type)30);
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_FMP1, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_TME, ENABLE);

    return MAL_ERROR_OK;
}

mal_error_e mal_can_direct_init(mal_can_init_s *init, const void *direct_init, mal_can_s *handle) {
	mal_error_e result;
	// Read direct init
	mal_hspec_stm32f0_can_direct_init_s *stm_direct_init;
	stm_direct_init = (mal_hspec_stm32f0_can_direct_init_s*)direct_init;
	// Initialize common part of CAN
	result = mal_hspec_stm32f0_can_init_common(init, handle);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	// Save call backs
    handle->tx_callback = init->tx_callback;
    handle->tx_callback_handle = init->tx_callback_handle;
    handle->rx_callback = init->rx_callback;
    handle->rx_callback_handle = init->rx_callback_handle;
	// Configure CAN
	CAN_InitTypeDef can_init;
	CAN_StructInit(&can_init);
	can_init.CAN_BS1 = stm_direct_init->bs1;
	can_init.CAN_BS2 = stm_direct_init->bs2;
	can_init.CAN_SJW = stm_direct_init->sjw;
	can_init.CAN_Prescaler = stm_direct_init->prescaler;
	can_init.CAN_ABOM = ENABLE;
	if (CAN_InitStatus_Success != CAN_Init(CAN, &can_init)) {
		return MAL_ERROR_INIT_FAILED;
	}
	// Enable interrupts
	// 30 equates to CAN_IRQ. However, the name of the constant changes based
	// on the MCU because it is not available on all of them. It is simpler to
	// use the constant directly. If the MCU does not support CAN, the code
	// will not get here.
	NVIC_EnableIRQ((IRQn_Type)30);
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_FMP1, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_TME, ENABLE);

	return MAL_ERROR_OK;
}

mal_error_e mal_can_deinit(mal_can_s *handle) {
	// Disable interrupt
    mal_can_disable_interrupt(handle, NULL);
	// Uninitialize interface
	CAN_DeInit(CAN);
    return MAL_ERROR_OK;
}

void CEC_CAN_IRQHandler(void) {
	// Check if transmit is empty
	if (CAN_GetITStatus(CAN, CAN_IT_TME) == SET) {
		if (can_handle->tx_callback != NULL) {
			mal_can_msg_s msg;
			if (MAL_ERROR_OK == can_handle->tx_callback(can_handle->tx_callback_handle, &msg)) {
				can_transmit_msg(&msg);
			} else {
				CAN_ITConfig(CAN, CAN_IT_TME, DISABLE);
				CAN_ClearITPendingBit(CAN, CAN_IT_TME);
				can_handle->interface_active = false;
			}
		} else {
			can_handle->interface_active = false;
		}
	}
	// Check FIFOs
	while (CAN_MessagePending(CAN, CAN_FIFO0)) {
		can_read_fifo(CAN_FIFO0);
	}
	while (CAN_MessagePending(CAN, CAN_FIFO1)) {
		can_read_fifo(CAN_FIFO1);
	}
}

static void can_read_fifo(uint8_t fifo) {
	// Read msg
	CanRxMsg can_msg;
	CAN_Receive(CAN, fifo, &can_msg);
	// Transfer msg
	mal_can_msg_s msg;
	if (CAN_Id_Standard == can_msg.IDE) {
		msg.id = can_msg.StdId;
		msg.id_type = MAL_CAN_ID_STANDARD;
	} else {
		msg.id = can_msg.ExtId;
		msg.id_type = MAL_CAN_ID_EXTENDED;
	}
	for (msg.size = 0; msg.size < can_msg.DLC; msg.size++) {
		msg.data[msg.size] = can_msg.Data[msg.size];
	}
	msg.size = can_msg.DLC;
	can_handle->rx_callback(can_handle->rx_callback_handle, &msg);
}

mal_error_e mal_can_transmit(mal_can_s *handle, mal_can_msg_s *msg) {
	mal_error_e result = MAL_ERROR_OK;
	// Disable interrupts to get true status of TX queue
    mal_can_interrupt_state_s state;
	mal_can_disable_interrupt(handle, &state);
	// Check if queue is empty
	if (!handle->interface_active) {
		handle->interface_active = true;
		can_transmit_msg(msg);
		CAN_ITConfig(CAN, CAN_IT_TME, ENABLE);
	} else {
		result = MAL_ERROR_HARDWARE_UNAVAILABLE;
	}

	mal_can_restore_interrupt(handle, &state);

	return result;
}

static void can_transmit_msg(mal_can_msg_s *msg) {
	CanTxMsg can_msg;
	if (MAL_CAN_ID_STANDARD == msg->id_type) {
		can_msg.StdId = msg->id;
		can_msg.IDE = CAN_Id_Standard;
	} else {
		can_msg.ExtId = msg->id;
		can_msg.IDE = CAN_Id_Extended;
	}
	for (can_msg.DLC = 0; can_msg.DLC < msg->size; can_msg.DLC++) {
		can_msg.Data[can_msg.DLC] = msg->data[can_msg.DLC];
	}
	can_msg.DLC = msg->size;
	can_msg.RTR = CAN_RTR_Data;
	CAN_Transmit(CAN, &can_msg);
}

mal_error_e mal_can_add_filter(mal_can_s *handle, mal_can_filter_s *filter) {
	mal_error_e result;
	// Disable interrupts
    mal_can_interrupt_state_s state;
	mal_can_disable_interrupt(handle, &state);
	// Find a free filter
	uint8_t filter_index;
	result = mal_hspec_stm_bcan_add_filter(&handle->can_filter_banks, filter, &filter_index);
	if (MAL_ERROR_OK == result) {
		// Initialise filter
		CAN_FilterInitTypeDef filter_init;
		filter_init.CAN_FilterActivation = ENABLE;
		if (MAL_HSPEC_STM_BCAN_FIFO_0 == handle->can_filter_banks.filter_banks[filter_index].fifo) {
            filter_init.CAN_FilterFIFOAssignment = CAN_FIFO0;
        } else {
            filter_init.CAN_FilterFIFOAssignment = CAN_FIFO1;
		}
		filter_init.CAN_FilterIdHigh = handle->can_filter_banks.filter_banks[filter_index].filter.std.id[1];
		filter_init.CAN_FilterIdLow = handle->can_filter_banks.filter_banks[filter_index].filter.std.id[0];
		filter_init.CAN_FilterMaskIdHigh = handle->can_filter_banks.filter_banks[filter_index].filter.std.mask[1];
		filter_init.CAN_FilterMaskIdLow = handle->can_filter_banks.filter_banks[filter_index].filter.std.mask[0];
		filter_init.CAN_FilterMode = CAN_FilterMode_IdMask;
		filter_init.CAN_FilterNumber = filter_index;
		if (MAL_CAN_ID_EXTENDED == filter->id_type) {
			filter_init.CAN_FilterScale = CAN_FilterScale_32bit;
		} else {
			filter_init.CAN_FilterScale = CAN_FilterScale_16bit;
		}
		CAN_FilterInit(&filter_init);
	}

	mal_can_restore_interrupt(handle, &state);

	return result;
}

mal_error_e mal_can_remove_filter(mal_can_s *handle, mal_can_filter_s *filter) {
	// Disable interrupts
    mal_can_interrupt_state_s state;
	mal_can_disable_interrupt(handle, &state);
	// Find filter index
	uint8_t filter_index;
	bool found = mal_hspec_stm_bcan_remove_filter(&handle->can_filter_banks, filter, &filter_index);
	if (found) {
		// Initialise filter
		CAN_FilterInitTypeDef filter_init;
		if (handle->can_filter_banks.filter_banks[filter_index].is_active) {
			filter_init.CAN_FilterActivation = ENABLE;
		} else {
			filter_init.CAN_FilterActivation = DISABLE;
		}
		filter_init.CAN_FilterFIFOAssignment = handle->can_filter_banks.filter_banks[filter_index].fifo;
		filter_init.CAN_FilterIdHigh = handle->can_filter_banks.filter_banks[filter_index].filter.std.id[1];
		filter_init.CAN_FilterIdLow = handle->can_filter_banks.filter_banks[filter_index].filter.std.id[0];
		filter_init.CAN_FilterMaskIdHigh = handle->can_filter_banks.filter_banks[filter_index].filter.std.mask[1];
		filter_init.CAN_FilterMaskIdLow = handle->can_filter_banks.filter_banks[filter_index].filter.std.mask[0];
		filter_init.CAN_FilterMode = CAN_FilterMode_IdMask;
		filter_init.CAN_FilterNumber = filter_index;
		if (MAL_CAN_ID_EXTENDED == filter->id_type) {
			filter_init.CAN_FilterScale = CAN_FilterScale_32bit;
		} else {
			filter_init.CAN_FilterScale = CAN_FilterScale_16bit;
		}
		CAN_FilterInit(&filter_init);
	}

	mal_can_restore_interrupt(handle, &state);

	return MAL_ERROR_OK;
}

MAL_DEFS_INLINE void mal_can_disable_interrupt(mal_can_s *handle, mal_can_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
	// 30 equates to CAN_IRQ. However, the name of the constant changes based
	// on the MCU because it is not available on all of them. It is simpler to
	// use the constant directly.
	state->active = NVIC_GetActive((IRQn_Type)30);
	// Enable interrupts
	NVIC_DisableIRQ((IRQn_Type)30);
	__DSB();
	__ISB();
}

MAL_DEFS_INLINE void mal_can_restore_interrupt(mal_can_s *handle, mal_can_interrupt_state_s *state) {
    MAL_DEFS_UNUSED(handle);
    if (state->active) {
        NVIC_EnableIRQ((IRQn_Type)30);
    }
}
