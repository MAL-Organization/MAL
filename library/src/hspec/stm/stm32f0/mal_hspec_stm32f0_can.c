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

#define CAN_FILTER_BANKS_SIZE	14
#define CAN_FILTER_STD_SIZE		2

typedef struct {
	uint32_t id;
	uint32_t mask;
} can_extended_filter_s;

typedef struct {
	uint16_t id[CAN_FILTER_STD_SIZE];
	uint16_t mask[CAN_FILTER_STD_SIZE];
} can_standard_filter_s;

typedef union {
	can_extended_filter_s ext;
	can_standard_filter_s std;
} can_filter_u;

typedef struct {
	can_filter_u filter;
	uint8_t filter_count;
	mal_hspec_can_id_type_e type;
	bool is_active;
	uint8_t fifo;
} can_filter_bank_s;

static void can_read_fifo(uint8_t fifo);

static void can_transmit_msg(mal_hspec_can_msg_s *msg);

static uint32_t can_extended_fr_format(uint32_t id);

static uint16_t can_standard_fr_format(uint16_t id);

static mal_hspec_can_tx_callback_t can_tx_callback = NULL;
static mal_hspec_can_rx_callback_t can_rx_callback = NULL;
static can_filter_bank_s can_filter_banks[CAN_FILTER_BANKS_SIZE];
static volatile bool interface_active = false;

mal_error_e mal_hspec_stm32f0_can_init(mal_hspec_can_init_s *init) {
	// Enable GPIO clocks
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->tx_gpio->port), ENABLE);
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->rx_gpio->port), ENABLE);
	// Configure alternate function
	mal_error_e result;
	uint8_t function;
	result = mal_hspec_stm32f0_get_pin_af(init->tx_gpio, MAL_HSPEC_STM32F0_AF_CAN_TX, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->tx_gpio->port), init->tx_gpio->pin, function);

	result = mal_hspec_stm32f0_get_pin_af(init->rx_gpio, MAL_HSPEC_STM32F0_AF_CAN_RX, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->rx_gpio->port), init->rx_gpio->pin, function);
	// Configure GPIOs
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->tx_gpio->pin);
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_OType = GPIO_OType_PP;
	gpio_init.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->tx_gpio->port), &gpio_init);

	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->rx_gpio->pin);
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->rx_gpio->port), &gpio_init);
	// Enable CAN clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);
	// Clear CAN
	CAN_DeInit(CAN);
	// Get APB Clock
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	// Compute bit time
	uint32_t prescaler;
	uint32_t tseg1;
	int32_t tseg2;
	uint32_t sjw;
	bool done = false;
	for (prescaler = 1; prescaler <= 1024; prescaler++) {
		// Calculate total time quantas
		uint32_t tq_total = (clocks.PCLK_Frequency) / (prescaler * init->bitrate);
		if (!tq_total) {
			continue;
		}
		// Remove tq to account for sync segment.
		tq_total--;
		// TSEG1 must be at least 2 time quantas long because of it includes
		// the propagation segment which takes 1 time quanta.
		for (tseg1 = 2; tseg1 < 16; tseg1++) {
			tseg2 = tq_total - tseg1;
			// Here are the rules to pass this point.
			// 1. TSEG2 must be equal or less to TSEG1. This is to ensure that
			//    the sample point is not before the 50% mark.
			if (tseg2 > tseg1) {
				continue;
			}
			// 2. Since TSEG2 must have a time, it cannot be 0.
			if (tseg2 <= 0) {
				continue;
			}
			// 3. TSEG2 has a maximum of 8 time quantas.
			if (tseg2 > 8) {
				continue;
			}
			// Limit error to 0.5%
			float tq = 1.0f / (((float)clocks.PCLK_Frequency) / (float)prescaler);
			float result_freq = 1.0f / (tq + tseg1 * tq + tq * tseg2);
			float error = fabsf(1.0f - (result_freq / (float)init->bitrate));
			if (error > 0.005f) {
				continue;
			}
			// We're, now we have to find a suitable synchronisation jump
			// width.
			for (sjw = 4; sjw >= 1; sjw--) {
				// Jump must not be longer than TSEG2 because the jump lengthen
				// or shorten TSEG2.
				if (sjw < tseg2) {
					done = true;
					break;
				}
			}
			if (done) {
				break;
			}
		}
		if (done) {
			break;
		}
	}
	if (!done) {
		return MAL_ERROR_CLOCK_ERROR;
	}
	// Save call backs
	can_tx_callback = init->tx_callback;
	can_rx_callback = init->rx_callback;
	// Configure CAN
	CAN_InitTypeDef can_init;
	CAN_StructInit(&can_init);
	can_init.CAN_BS1 = tseg1 - 1;
	can_init.CAN_BS2 = tseg2 - 1;
	can_init.CAN_SJW = sjw - 1;
	can_init.CAN_Prescaler = prescaler;
	can_init.CAN_ABOM = ENABLE;
	if (CAN_InitStatus_Success != CAN_Init(CAN, &can_init)) {
		return MAL_ERROR_INIT_FAILED;
	}
	// Enable interrupts
	// 30 equates to CAN_IRQ. However, the name of the constant changes based
	// on the MCU because it is not available on all of them. It is simpler to
	// use the constant directly. If the MCU does not support CAN, the code
	// will not get here.
	NVIC_EnableIRQ(30);
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_FMP1, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_TME, ENABLE);
}

void CEC_CAN_IRQHandler(void) {
	// Check if transmit is empty
	if (CAN_GetITStatus(CAN, CAN_IT_TME) == SET) {
		if (can_tx_callback != NULL) {
			mal_hspec_can_msg_s msg;
			if (MAL_ERROR_OK == can_tx_callback(MAL_HSPEC_CAN_1, &msg)) {
				can_transmit_msg(&msg);
			} else {
				CAN_ITConfig(CAN, CAN_IT_TME, DISABLE);
				CAN_ClearITPendingBit(CAN, CAN_IT_TME);
				interface_active = false;
			}
		} else {
			interface_active = false;
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
	mal_hspec_can_msg_s msg;
	if (CAN_Id_Standard == can_msg.IDE) {
		msg.id = can_msg.StdId;
		msg.id_type = MAL_HSPEC_CAN_ID_STANDARD;
	} else {
		msg.id = can_msg.ExtId;
		msg.id_type = MAL_HSPEC_CAN_ID_EXTENDED;
	}
	for (msg.size = 0; msg.size < can_msg.DLC; msg.size++) {
		msg.data[msg.size] = can_msg.Data[msg.size];
	}
	msg.size = can_msg.DLC;
	can_rx_callback(MAL_HSPEC_CAN_1, &msg);
}

mal_error_e mal_hspec_stm32f0_can_transmit(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg) {
	if (MAL_HSPEC_CAN_1 != interface) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	mal_error_e result = MAL_ERROR_OK;;
	// Disable interrupts to get true status of TX queue
	bool active = mal_hspec_stm32f0_disable_can_interrupt(interface);
	// Check if queue is empty
	if (!interface_active) {
		interface_active = true;
		can_transmit_msg(msg);
		CAN_ITConfig(CAN, CAN_IT_TME, ENABLE);
	} else {
		result = MAL_ERROR_HARDWARE_UNAVAILABLE;
	}

	mal_hspec_stm32f0_enable_can_interrupt(interface, active);

	return result;
}

static void can_transmit_msg(mal_hspec_can_msg_s *msg) {
	CanTxMsg can_msg;
	if (MAL_HSPEC_CAN_ID_STANDARD == msg->id_type) {
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

mal_error_e mal_hspec_stm32f0_can_add_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter) {
	static uint16_t fifo = CAN_Filter_FIFO0;
	if (MAL_HSPEC_CAN_1 != interface) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	mal_error_e result = MAL_ERROR_OK;
	// Disable interrupts
	bool active = mal_hspec_stm32f0_disable_can_interrupt(interface);
	// Find a free filter
	uint8_t filter_index;
	bool found = false;
	for (filter_index = 0; filter_index < CAN_FILTER_BANKS_SIZE; filter_index++) {
		// Check if filter is unused.
		if (!can_filter_banks[filter_index].is_active) {
			found = true;
			break;
		}
		// If the ID is standard, it is possible to resuse that filter if it
		// has an available slot
		if (MAL_HSPEC_CAN_ID_STANDARD == filter->id_type && MAL_HSPEC_CAN_ID_STANDARD == can_filter_banks[filter_index].type) {
			if (can_filter_banks[filter_index].filter_count < CAN_FILTER_STD_SIZE) {
				found = true;
				break;
			}
		}
	}
	if (!found) {
		result =  MAL_ERROR_HARDWARE_UNAVAILABLE;
	} else {
		// Initialise filter array
		if (MAL_HSPEC_CAN_ID_EXTENDED == filter->id_type) {
			can_filter_banks[filter_index].is_active = true;
			can_filter_banks[filter_index].type = MAL_HSPEC_CAN_ID_EXTENDED;
			can_filter_banks[filter_index].filter.ext.id = can_extended_fr_format(filter->id);
			can_filter_banks[filter_index].filter.ext.mask = can_extended_fr_format(filter->mask);
		} else {
			// Check if filter is already active to reset count at the same time.
			if (!can_filter_banks[filter_index].is_active) {
				can_filter_banks[filter_index].filter_count = 0;
				can_filter_banks[filter_index].is_active = true;
			}
			// Set correct mask and filter
			can_filter_banks[filter_index].filter.std.id[can_filter_banks[filter_index].filter_count] = can_standard_fr_format(filter->id);
			can_filter_banks[filter_index].filter.std.mask[can_filter_banks[filter_index].filter_count] = can_standard_fr_format(filter->mask);
			can_filter_banks[filter_index].filter_count++;
			// Fill remaining filters with the last one
			uint32_t i;
			for (i = can_filter_banks[filter_index].filter_count; i < CAN_FILTER_STD_SIZE; i++) {
				can_filter_banks[filter_index].filter.std.id[i] = can_standard_fr_format(filter->id);
				can_filter_banks[filter_index].filter.std.mask[i] = can_standard_fr_format(filter->mask);
			}
		}
		can_filter_banks[filter_index].fifo = fifo;
		// Initialise filter
		CAN_FilterInitTypeDef filter_init;
		filter_init.CAN_FilterActivation = ENABLE;
		filter_init.CAN_FilterFIFOAssignment = fifo;
		filter_init.CAN_FilterIdHigh = can_filter_banks[filter_index].filter.std.id[1];
		filter_init.CAN_FilterIdLow = can_filter_banks[filter_index].filter.std.id[0];
		filter_init.CAN_FilterMaskIdHigh = can_filter_banks[filter_index].filter.std.mask[1];
		filter_init.CAN_FilterMaskIdLow = can_filter_banks[filter_index].filter.std.mask[0];
		filter_init.CAN_FilterMode = CAN_FilterMode_IdMask;
		filter_init.CAN_FilterNumber = filter_index;
		if (MAL_HSPEC_CAN_ID_EXTENDED == filter->id_type) {
			filter_init.CAN_FilterScale = CAN_FilterScale_32bit;
		} else {
			filter_init.CAN_FilterScale = CAN_FilterScale_16bit;
		}
		CAN_FilterInit(&filter_init);
		// Switch fifo
		if (CAN_Filter_FIFO0 == fifo) {
			fifo = CAN_Filter_FIFO1;
		} else {
			fifo = CAN_Filter_FIFO0;
		}
	}

	mal_hspec_stm32f0_enable_can_interrupt(MAL_HSPEC_CAN_1, active);

	return result;
}

static uint32_t can_extended_fr_format(uint32_t id) {
	uint32_t result = 0;
	// 11 MSb are the 11 bits of the standard ID (first 11 bits of
	// extended ID)
	result |= (id & 0x3FF) << 21;
	// Next 18 bits is the MS part of the extended ID
	result |= (id & 0x1FFFF800) << 3;
	// Next and last 3 bits are flags. IDE which is set since this is
	// extended. RTR not set since we don't support remote frames. And
	// the last one must be 0.
	result |= 4;

	return result;
}

static uint16_t can_standard_fr_format(uint16_t id) {
	uint16_t result = 0;
	// 11 MSb are the 11 bits of the standard ID (first 11 bits of
	// extended ID)
	result |= (id & 0x3FF) << 5;
	// Next and last 5 bits are flags. RTR which is not set since we don't
	// support remote frames. IDE which is not set since this is standard. And
	// the last 3 must be 0 since they are only valid in extended.
	result |= 0;

	return result;
}

mal_error_e mal_hspec_stm32f0_can_remove_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter) {
	static uint16_t fifo = CAN_Filter_FIFO0;
	if (MAL_HSPEC_CAN_1 != interface) {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	mal_error_e result = MAL_ERROR_OK;;
	// Format id and mask
	if (MAL_HSPEC_CAN_ID_STANDARD == filter->id_type) {
		filter->id = can_standard_fr_format(filter->id);
		filter->mask = can_standard_fr_format(filter->mask);
	} else {
		filter->id = can_extended_fr_format(filter->id);
		filter->mask = can_extended_fr_format(filter->mask);
	}
	// Disable interrupts
	bool active = mal_hspec_stm32f0_disable_can_interrupt(interface);
	// Find filter index
	uint8_t filter_index;
	uint8_t std_filter_index;
	bool found = false;
	for (filter_index = 0; filter_index < CAN_FILTER_BANKS_SIZE; filter_index++) {
		// Check if filter is unused.
		if (!can_filter_banks[filter_index].is_active) {
			continue;
		}

		if (MAL_HSPEC_CAN_ID_STANDARD == filter->id_type && MAL_HSPEC_CAN_ID_STANDARD == can_filter_banks[filter_index].type) {
			int32_t i;
			for (i = can_filter_banks[filter_index].filter_count - 1; i >=0; i--) {
				if (can_filter_banks[filter_index].filter.std.id[i] == filter->id && can_filter_banks[filter_index].filter.std.mask[i] == filter->mask) {
					std_filter_index = i;
					found = true;
					break;
				}
			}
		} else if (MAL_HSPEC_CAN_ID_EXTENDED == filter->id_type && MAL_HSPEC_CAN_ID_EXTENDED == can_filter_banks[filter_index].type) {
			if (can_filter_banks[filter_index].filter.ext.id == filter->id && can_filter_banks[filter_index].filter.ext.mask == filter->mask) {
				found = true;
			}
		}
		if (found) {
			break;
		}
	}
	if (found) {
		// Free filter array, especially for santard type
		if (MAL_HSPEC_CAN_ID_EXTENDED == filter->id_type) {
			can_filter_banks[filter_index].is_active = false;
		} else {
			if (can_filter_banks[filter_index].filter_count <= 1) {
				can_filter_banks[filter_index].is_active = false;
			} else {
				uint8_t copy_index = 0;
				if (std_filter_index == 0) {
					copy_index = 1;
				}
				// Set correct mask and filter
				can_filter_banks[filter_index].filter.std.id[std_filter_index] = can_filter_banks[filter_index].filter.std.id[copy_index];
				can_filter_banks[filter_index].filter.std.mask[std_filter_index] = can_filter_banks[filter_index].filter.std.mask[copy_index];
				can_filter_banks[filter_index].filter_count--;
			}
		}
		// Initialise filter
		CAN_FilterInitTypeDef filter_init;
		if (can_filter_banks[filter_index].is_active) {
			filter_init.CAN_FilterActivation = ENABLE;
		} else {
			filter_init.CAN_FilterActivation = DISABLE;
		}
		filter_init.CAN_FilterFIFOAssignment = can_filter_banks[filter_index].fifo;
		filter_init.CAN_FilterIdHigh = can_filter_banks[filter_index].filter.std.id[1];
		filter_init.CAN_FilterIdLow = can_filter_banks[filter_index].filter.std.id[0];
		filter_init.CAN_FilterMaskIdHigh = can_filter_banks[filter_index].filter.std.mask[1];
		filter_init.CAN_FilterMaskIdLow = can_filter_banks[filter_index].filter.std.mask[0];
		filter_init.CAN_FilterMode = CAN_FilterMode_IdMask;
		filter_init.CAN_FilterNumber = filter_index;
		if (MAL_HSPEC_CAN_ID_EXTENDED == filter->id_type) {
			filter_init.CAN_FilterScale = CAN_FilterScale_32bit;
		} else {
			filter_init.CAN_FilterScale = CAN_FilterScale_16bit;
		}
		CAN_FilterInit(&filter_init);
	}

	mal_hspec_stm32f0_enable_can_interrupt(MAL_HSPEC_CAN_1, active);

	return result;
}

bool mal_hspec_stm32f0_disable_can_interrupt(mal_hspec_can_e interface) {
	// 30 equates to CAN_IRQ. However, the name of the constant changes based
	// on the MCU because it is not available on all of them. It is simpler to
	// use the constant directly.
	bool active = NVIC_GetActive(30);
	// Enable interrupts
	NVIC_DisableIRQ(30);
	__DSB();
	__ISB();
	return active;
}
