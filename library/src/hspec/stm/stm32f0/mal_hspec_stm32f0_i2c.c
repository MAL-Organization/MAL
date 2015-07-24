/*
 * mal_hspec_stm32f0_i2c.c
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

#include "mal_hspec_stm32f0_i2c.h"
#include "std/mal_stdlib.h"

#define I2C_MIN_AF_DELAY		0.00000005f

typedef enum {
	I2C_STATE_START,
	I2C_STATE_RECEIVING,
	I2C_STATE_ERROR,
	I2C_STATE_WAITING_TRANSFER_COMPLETE,
	I2C_STATE_TRANSMITTING,
	I2C_STATE_WAIT_STOP
} i2c_states_e;

typedef struct {
	mal_hspec_i2c_msg_s *msg;
	bool is_active;
	volatile i2c_states_e state;
	volatile uint8_t data_ptr;
	volatile mal_hspec_i2c_cmd_e cmd;
	I2C_TypeDef *stm_handle;
} i2c_handle_s;

static void i2c_start_transfer(i2c_handle_s *i2c_handle);

static void i2c_interrupt_transmit_handler(i2c_handle_s *handle);

static void i2c_interrupt_receive_handler(i2c_handle_s *handle);

static void i2c_common_errors(i2c_handle_s *handle);

static void i2c_common(i2c_handle_s *handle);

static i2c_handle_s i2c_handle_1 = {
	.is_active = false,
	.stm_handle = I2C1
};

static i2c_handle_s i2c_handle_2 = {
	.is_active = false,
	.stm_handle = I2C2
};

mal_error_e mal_hspec_stm32f0_i2c_master_init(mal_hspec_i2c_init_s *init) {
	uint64_t i2c_clock;
	// Set I2C clock source
	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	// See STM32F0 user manual for I2C clock requirements.
	uint64_t min_clk = ((float)(8 * init->bitrate)) / (1.0f - 0.00000052f * ((float)init->bitrate));
	if (MAL_HSPEC_I2C_1 == init->interface) {
		if (SystemCoreClock > min_clk) {
			RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
			i2c_clock = SystemCoreClock;
		} else if (HSI_VALUE > min_clk) {
			RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);
			i2c_clock = HSI_VALUE;
		} else {
			return MAL_ERROR_CLOCK_ERROR;
		}
	} else if (MAL_HSPEC_I2C_2 == init->interface) {
		i2c_clock = clocks.PCLK_Frequency;
		if (clocks.PCLK_Frequency < min_clk) {
			return MAL_ERROR_CLOCK_ERROR;
		}
	} else {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	if (init->bitrate >= ((4 * clocks.PCLK_Frequency) / 3)) {
		return MAL_ERROR_CLOCK_ERROR;
	}
	// Enable GPIO clocks
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->scl_gpio->port), ENABLE);
	RCC_AHBPeriphClockCmd(mal_hspec_stm32f0_get_rcc_gpio_port(init->sda_gpio->port), ENABLE);
	// Enable I2C clock
	if (MAL_HSPEC_I2C_1 == init->interface) {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	} else {
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	}
	// Select correct I2C pin functions
	mal_hspec_stm32f0_af_e scl_af;
	mal_hspec_stm32f0_af_e sda_af;
	if (MAL_HSPEC_I2C_1 == init->interface) {
		scl_af = MAL_HSPEC_STM32F0_AF_I2C1_SCL;
		sda_af = MAL_HSPEC_STM32F0_AF_I2C1_SDA;
	} else {
		scl_af = MAL_HSPEC_STM32F0_AF_I2C2_SCL;
		sda_af = MAL_HSPEC_STM32F0_AF_I2C2_SDA;
	}
	// Configure alternate function
	mal_error_e result;
	uint8_t function;
	result = mal_hspec_stm32f0_get_pin_af(init->scl_gpio, scl_af, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->scl_gpio->port), init->scl_gpio->pin, function);

	result = mal_hspec_stm32f0_get_pin_af(init->sda_gpio, sda_af, &function);
	if (MAL_ERROR_OK != result) {
		return result;
	}
	GPIO_PinAFConfig(mal_hspec_stm32f0_get_gpio_typedef(init->sda_gpio->port), init->sda_gpio->pin, function);
	// Configure GPIOs
	GPIO_InitTypeDef gpio_init;
	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->scl_gpio->pin);
	gpio_init.GPIO_Mode = GPIO_Mode_AF;
	gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
	gpio_init.GPIO_OType = GPIO_OType_OD;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->scl_gpio->port), &gpio_init);

	gpio_init.GPIO_Pin = MAL_HSPEC_STM32F0_GET_GPIO_PIN(init->sda_gpio->pin);
	GPIO_Init(mal_hspec_stm32f0_get_gpio_typedef(init->sda_gpio->port), &gpio_init);
	// Compute timing
	// Determine fall time
	float tf = 0.0000003; // 300 ns
	if (init->bitrate > 400000) {
		// 12 ns if bitrate is higher than 400k
		tf = 0.00000012;
	}
	// Determine rise time
	float tr = 0.000001; // 1us
	if (init->bitrate > 100000 && init->bitrate <= 400000) {
		// If speed is fast mode (between 100k and 400k), 300ns
		tr = 0.0000003;
	} else if (init->bitrate > 400000) {
		// 120ns if faster.
		tr = 0.00000012;
	}
	// Determine minimum setup time
	float tsu = 0.00000015; // 250ns
	if (init->bitrate > 100000 && init->bitrate <= 400000) {
		// If speed is fast mode (between 100k and 400k), 100ns
		tsu = 0.0000001;
	} else if (init->bitrate > 400000) {
		// 50ns if faster.
		tsu = 0.00000005;
	}
	// Determine valid data time
	float tvd = 0.00000345; // 3.45us
	if (init->bitrate > 100000 && init->bitrate <= 400000) {
		// If speed is fast mode (between 100k and 400k), 900ns
		tsu = 0.0000009;
	} else if (init->bitrate > 400000) {
		// 450ns if faster.
		tsu = 0.00000045;
	}
	// Determine true half period. Bitrate = Fall Time + Time Low + Rise Time + Time High
	float half_period = ((1.0f / ((float)init->bitrate)) - tf - tr) / 2;
	// Try prescaller values to find a match to all values
	uint32_t presc;
	uint32_t scldel;
	uint32_t sdadel;
	uint32_t scllh;
	for (presc = 0; presc <= 15; presc++) {
		// Compute prescaled period
		float ti2c = (1.0f / ((float)i2c_clock));
		float tpresc = ti2c * (presc + 1);
		// Compute SCL delay for setup time
		scldel = ((tr + tsu) / tpresc) - 1;
		if (scldel > 15) {
			// SCLDEL is only 4 bits long, must use another prescaler value
			continue;
		}
		// Compute SDA delay for hold time. The formula uses minimum analog
		// filter delay which is 50ns.
		sdadel = (tf - I2C_MIN_AF_DELAY - 3 * ti2c) / tpresc;
		if (sdadel > 15) {
			// SDADEL is only 4 bits long, must use another prescaler value
			continue;
		}
		// Compute SCL low and high time
		scllh = ((half_period - I2C_MIN_AF_DELAY - 2 * ti2c) / tpresc) - 1;
		if (scllh > 255) {
			// SCLH and SCLL are 8 bits
			continue;
		}
		// All settings are good, end loop
		break;
	}
	// Get I2C typedef
	I2C_TypeDef *i2c_typedef;
	if (MAL_HSPEC_I2C_1 == init->interface) {
		i2c_typedef = I2C1;
	} else {
		i2c_typedef = I2C2;
	}
	// Configure I2C
	I2C_InitTypeDef i2c_init;
	i2c_init.I2C_Mode = I2C_Mode_I2C;
	i2c_init.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	i2c_init.I2C_DigitalFilter = 0;
	i2c_init.I2C_OwnAddress1 = 0;
	i2c_init.I2C_Ack = I2C_Ack_Enable;
	i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c_init.I2C_Timing = (presc << 28) | (scldel << 20) | (sdadel << 16) | (scllh << 8) | scllh;
	I2C_Init(i2c_typedef, &i2c_init);
	// Enable interrupt
	if (MAL_HSPEC_I2C_1 == init->interface) {
		NVIC_EnableIRQ(I2C1_IRQn);
	} else {
		NVIC_EnableIRQ(I2C2_IRQn);
	}
	I2C_ITConfig(i2c_typedef, I2C_IT_RXI|I2C_IT_TXI|I2C_IT_TCI|I2C_IT_NACKI|I2C_IT_ERRI|I2C_IT_STOPI, ENABLE);
	// Enable I2C, finally!
	I2C_Cmd(i2c_typedef, ENABLE);

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_i2c_transfer(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg) {
	i2c_handle_s *i2c_handle;
	// Fetch I2C handle
	if (MAL_HSPEC_I2C_1 == interface) {
		i2c_handle = &i2c_handle_1;
	} else if (MAL_HSPEC_I2C_2 == interface) {
		i2c_handle = &i2c_handle_2;
	} else {
		return MAL_ERROR_HARDWARE_INVALID;
	}
	// Make sure I2C is free
	if (i2c_handle->is_active) {
		return MAL_ERROR_HARDWARE_UNAVAILABLE;
	}
	// Save msg
	i2c_handle->msg = msg;

	i2c_start_transfer(i2c_handle);

	return MAL_ERROR_OK;
}

void i2c_start_transfer(i2c_handle_s *i2c_handle) {
	i2c_handle->cmd = i2c_handle->msg->packet.cmd;
	// Start transfer
	uint32_t transfer_type = I2C_Generate_Start_Read;
	if (MAL_HSPEC_I2C_WRITE == i2c_handle->cmd) {
		transfer_type = I2C_Generate_Start_Write;
	}
	I2C_TransferHandling(i2c_handle->stm_handle, i2c_handle->msg->packet.address, i2c_handle->msg->packet.packet_size, I2C_SoftEnd_Mode, transfer_type);
}

void I2C1_IRQHandler(void) {
	if (MAL_HSPEC_I2C_WRITE == i2c_handle_1.cmd) {
		i2c_interrupt_transmit_handler(&i2c_handle_1);
	} else {
		i2c_interrupt_receive_handler(&i2c_handle_1);
	}
}

void I2C2_IRQHandler(void) {
	if (MAL_HSPEC_I2C_WRITE == i2c_handle_2.cmd) {
		i2c_interrupt_transmit_handler(&i2c_handle_2);
	} else {
		i2c_interrupt_receive_handler(&i2c_handle_2);
	}
}

void i2c_interrupt_transmit_handler(i2c_handle_s *handle) {
	// Common errors
	i2c_common_errors(handle);
	// Nack event
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_NACKF) == SET) {
		// Clear interrupt
		I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_NACKF);
		if (handle->state == I2C_STATE_TRANSMITTING) {
			mal_hspec_i2c_result_e result;
			// Check if write is complete
			if (handle->data_ptr >= handle->msg->packet.packet_size) {
				// Change state
				result = MAL_HSPEC_I2C_NACK_COMPLETE;
			} else {
				// Transfer is incomplete
				result = MAL_HSPEC_I2C_NACK_INCOMPLETE;
			}
			handle->msg->callback(&handle->msg->packet, result, &handle->msg);
			// Next state
			handle->state = I2C_STATE_WAIT_STOP;
		} else {
			// Unknown sate for nack
			handle->msg->callback(&handle->msg->packet, MAL_HSPEC_I2C_NACK_INCOMPLETE, &handle->msg);
			// Next state
			handle->state = I2C_STATE_WAIT_STOP;
		}
	}
	// Transmitter status
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_TXIS) == SET) {
		// Make sure we are receiving
		if (I2C_STATE_TRANSMITTING != handle->state) {
			// Stop receiving
			I2C_GenerateSTOP(handle->stm_handle, ENABLE);
			// We are in error
			handle->state = I2C_STATE_ERROR;
			// Clear interrupt
			I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_TXIS);
		} else {
			// Check if there still data to be sent
			if (handle->data_ptr >= handle->msg->packet.packet_size) {
				// Clear interrupt
				I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_TXIS);
				// Flag error
				handle->state = I2C_STATE_WAITING_TRANSFER_COMPLETE;
			} else {
				// Transmit next data
				I2C_SendData(handle->stm_handle, handle->msg->packet.buffer[handle->data_ptr++]);
			}
		}
	}
	// Common states
	i2c_common(handle);
}

void i2c_interrupt_receive_handler(i2c_handle_s *handle) {
	// Common errors
	i2c_common_errors(handle);
	// Receiver status
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_RXNE) == SET) {
		// Make sure we are receiving
		if (I2C_STATE_RECEIVING != handle->state) {
			// Clear interrupt
			I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_RXNE);
			// Stop receiving
			I2C_GenerateSTOP(handle->stm_handle, ENABLE);
			// We are in error
			handle->state = I2C_STATE_ERROR;
		} else {
			// Read data, also clears interrupt
			handle->msg->packet.buffer[handle->data_ptr++] = I2C_ReceiveData(handle->stm_handle);
			// Check if data is complete
			if (handle->data_ptr >= handle->msg->packet.packet_size) {
				// Change state
				handle->state = I2C_STATE_WAITING_TRANSFER_COMPLETE;
			}
		}
	}
	// Common states
	i2c_common(handle);
}

void i2c_common_errors(i2c_handle_s *handle) {
	// Bus error
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_BERR) == SET) {
		// Clear interrupt
		I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_BERR);
		// Stop bus
		I2C_GenerateSTOP(handle->stm_handle, ENABLE);
		// Start or stop out of place switch to error state
		handle->state = I2C_STATE_ERROR;
	}
	// Arbitration loss error
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_ARLO) == SET) {
		// Clear interrupt
		I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_ARLO);
		// Start or stop out of place switch to error state
		handle->state = I2C_STATE_ERROR;
	}
}

void i2c_common(i2c_handle_s *handle) {
	// Check if transmission is complete
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_TCR) == SET) {
		// Transmit complete, stop transfer, clear interrupt with stop
		I2C_GenerateSTOP(handle->stm_handle, ENABLE);
		// Make sure it was expected
		if (I2C_STATE_WAITING_TRANSFER_COMPLETE != handle->state) {
			// We are in error
			handle->state = I2C_STATE_ERROR;
		} else {
			handle->msg->callback(&handle->msg->packet, MAL_HSPEC_I2C_SUCCESS, &handle->msg);
			// Next state
			handle->state = I2C_STATE_WAIT_STOP;
		}
	}
	// Check for stop
	if (I2C_GetITStatus(handle->stm_handle, I2C_IT_STOPF) == SET) {
		// Clear interrupt
		I2C_ClearITPendingBit(handle->stm_handle, I2C_IT_STOPF);
		// Check if stop is expected
		if (I2C_STATE_WAIT_STOP != handle->state) {
			handle->msg->callback(&handle->msg->packet, MAL_HSPEC_I2C_BUS_ERROR, &handle->msg);
		}
		// Check if a new message can be started
		if (handle->msg != NULL) {
			i2c_start_transfer(handle);
		} else {
			handle->is_active = false;
		}
	}
}
