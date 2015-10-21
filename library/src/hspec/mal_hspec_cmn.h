/**
 * @file mal_hspec_cmn.h
 * @author Olivier Allaire
 * @date May 3 2015
 * @copyright Copyright (c) 2015 Olivier Allaire
 * @par This file is part of MAL.
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
 * @brief Common definitions to prevent circular dependencies.
 */

#ifndef HSPEC_MAL_HSPEC_CMN_H_
#define HSPEC_MAL_HSPEC_CMN_H_

#include "std/mal_stdint.h"
#include "std/mal_error.h"

/**
 * @addtogroup Clock
 * @{
 */

/**
 * @brief Possible clock sources for the MCU
 */
typedef enum {
	MAL_HSPEC_SYSTEM_CLK_SRC_INTERNAL,//!< Source is an internal oscillator.
	MAL_HSPEC_SYSTEM_CLK_SRC_EXTERNAL,//!< Source is an external crystal or oscillator.
	MAL_HSPEC_SYSTEM_CLK_SRC_AUTO     //!< MAL will select the source. Note it is safer to define a known source.
} mal_hspec_system_clk_src_e;

/**
 * @brief Structure to define the system clock source.
 */
typedef struct {
	uint64_t frequency; /**< The frequency in hertz.*/
	mal_hspec_system_clk_src_e src; /**< The source of the frequency.*/
} mal_hspec_system_clk_s;

/**
 * @}
 */

/**
 * @addtogroup GPIO
 * @{
 */

/**
 * Possible GPIO ports.
 */
typedef enum {
	MAL_HSPEC_PORT_A = 0,  //!< MAL_HSPEC_PORT_A
	MAL_HSPEC_PORT_B = 1,  //!< MAL_HSPEC_PORT_B
	MAL_HSPEC_PORT_C = 2,  //!< MAL_HSPEC_PORT_C
	MAL_HSPEC_PORT_D = 3,  //!< MAL_HSPEC_PORT_D
	MAL_HSPEC_PORT_E = 4,  //!< MAL_HSPEC_PORT_E
	MAL_HSPEC_PORT_F = 5,  //!< MAL_HSPEC_PORT_F
	MAL_HSPEC_PORT_SIZE = 6//!< MAL_HSPEC_PORT_SIZE
} mal_hspec_port_e;

/**
 * Defines a GPIO.
 */
typedef struct {
	mal_hspec_port_e port; /**< The port of the GPIO.*/
	uint8_t pin; /**< The pin of the GPIO.*/
} mal_hspec_gpio_s;

/**
 * Possible GPIO directions.
 */
typedef enum {
	MAL_GPIO_DIR_IN,//!< MAL_GPIO_DIR_IN
	MAL_GPIO_DIR_OUT//!< MAL_GPIO_DIR_OUT
} mal_hspec_gpio_dir_e;

/**
 * Possible GPIO types.
 */
typedef enum {
	MAL_GPIO_OUT_PP,//!< Push-Pull
	MAL_GPIO_OUT_OD //!< Open Drain
} mal_hspec_gpio_out_e;

/**
 * Possible pull-up and pull-down values.
 */
typedef enum {
	MAL_GPIO_PUPD_NONE,//!< No pull-up or pull-down.
	MAL_GPIO_PUPD_PU,  //!< Pull-up.
	MAL_GPIO_PUPD_PD   //!< Pull-down.
} mal_hspec_gpio_pupd_e;

/**
 * Parameters to initialise a GPIO.
 */
typedef struct {
	mal_hspec_gpio_s gpio; /**< The gpio to initialise.*/
	mal_hspec_gpio_dir_e direction; /**< The direction of the GPIO.*/
	mal_hspec_gpio_out_e output_config; /**< The output configuration (type) of the GPIO.*/
	mal_hspec_gpio_pupd_e pupd; /**< The pull-up and pull-down configuration.*/
	uint64_t speed; /**< The refresh speed of the GPIO in hertz.*/
} mal_hspec_gpio_init_s;

/**
 * Possible GPIO event triggers.
 */
typedef enum {
	MAL_HSPEC_GPIO_EVENT_RISING, //!< MAL_HSPEC_GPIO_EVENT_RISING
	MAL_HSPEC_GPIO_EVENT_FALLING,//!< MAL_HSPEC_GPIO_EVENT_FALLING
	MAL_HSPEC_GPIO_EVENT_BOTH    //!< MAL_HSPEC_GPIO_EVENT_BOTH
} mal_hspec_gpio_event_e;

/**
 * @brief GPIO event callback.
 * @return Return a satus of the callback.
 */
typedef mal_error_e (*mal_hspec_gpio_event_callback_t)(void);

/**
 * Parameters to initialise an event.
 */
typedef struct {
	const mal_hspec_gpio_s *gpio; /**< The gpio for the event.*/
	mal_hspec_gpio_event_e event; /**< The event that will trigger.*/
	mal_hspec_gpio_event_callback_t callback; /**< The callback to execute upon event.*/
} mal_hspec_gpio_event_init_s;

/**
 * @}
 */

/**
 * @addtogroup Timer
 * @{
 */

/**
 * Possible timers to use. Note that not all timers are supported on all
 * platforms.
 */
typedef enum {
	MAL_HSPEC_TIMER_1 = 0,    //!< Timer 1
	MAL_HSPEC_TIMER_2 = 1,    //!< Timer 2
	MAL_HSPEC_TIMER_3 = 2,    //!< Timer 3
	MAL_HSPEC_TIMER_4 = 3,    //!< Timer 4
	MAL_HSPEC_TIMER_5 = 4,    //!< Timer 5
	MAL_HSPEC_TIMER_6 = 5,    //!< Timer 6
	MAL_HSPEC_TIMER_7 = 6,    //!< Timer 7
	MAL_HSPEC_TIMER_8 = 7,    //!< Timer 8
	MAL_HSPEC_TIMER_9 = 8,    //!< Timer 9
	MAL_HSPEC_TIMER_10 = 9,   //!< Timer 10
	MAL_HSPEC_TIMER_11 = 10,  //!< Timer 11
	MAL_HSPEC_TIMER_12 = 11,  //!< Timer 12
	MAL_HSPEC_TIMER_13 = 12,  //!< Timer 13
	MAL_HSPEC_TIMER_14= 13,   //!< Timer 14
	MAL_HSPEC_TIMER_15 = 14,  //!< Timer 15
	MAL_HSPEC_TIMER_16 = 15,  //!< Timer 16
	MAL_HSPEC_TIMER_17 = 16,  //!< Timer 17
	MAL_HSPEC_TIMER_SIZE = 17,//!< Do not use.
	MAL_HSPEC_TIMER_ANY       //!< Use to pick first available timer for initialisation.
} mal_hspec_timer_e;

/**
 * Possible timer modes.
 */
typedef enum {
	MAL_HSPEC_TIMER_MODE_TICK,//!< MAL_HSPEC_TIMER_MODE_TICK
	MAL_HSPEC_TIMER_MODE_TASK //!< MAL_HSPEC_TIMER_MODE_TASK
} mal_hspec_timer_mode_e;

/**
 * @brief Function pointer typdef for timer in task mode.
 * @param timer Will provide the timer executing the callback.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_hspec_timer_callback_t)(mal_hspec_timer_e timer);

/**
 * @}
 */
// Serial

typedef enum {
	MAL_HSPEC_SERIAL_PORT_1 = 0,  //!< MAL_HSPEC_SERIAL_PORT_1
	MAL_HSPEC_SERIAL_PORT_2 = 1,  //!< MAL_HSPEC_SERIAL_PORT_2
	MAL_HSPEC_SERIAL_PORT_SIZE = 2//!< MAL_HSPEC_SERIAL_PORT_SIZE
} mal_hspec_serial_port_e;

typedef enum {
	MAL_HSPEC_SERIAL_DATA_7_BITS,
	MAL_HSPEC_SERIAL_DATA_8_BITS,
	MAL_HSPEC_SERIAL_DATA_9_BITS
} mal_hspec_serial_data_size_e;

typedef enum {
	MAL_HSPEC_SERIAL_STOP_BITS_1,
	MAL_HSPEC_SERIAL_STOP_BITS_2
} mal_hspec_serial_stop_bits_e;

typedef enum {
	MAL_HSPEC_SERIAL_PARITY,
	MAL_HSPEC_SERIAL_NO_PARITY
} mal_hspec_serial_parity_e;

typedef mal_error_e (*mal_hspec_serial_tx_callbacl_t)(uint16_t *data);

typedef mal_error_e (*mal_hspec_serial_rx_callbacl_t)(uint16_t data);

typedef struct {
	mal_hspec_serial_port_e port;
	mal_hspec_gpio_s *rx_gpio;
	mal_hspec_gpio_s *tx_gpio;
	uint64_t baudrate;
	mal_hspec_serial_data_size_e data_size;
	mal_hspec_serial_stop_bits_e stop_bits;
	mal_hspec_serial_parity_e parity;
	mal_hspec_serial_tx_callbacl_t tx_callback;
	mal_hspec_serial_rx_callbacl_t rx_callback;
} mal_hspec_serial_init_s;

// I2C

typedef enum {
	MAL_HSPEC_I2C_1 = 0,
	MAL_HSPEC_I2C_2 = 1,
	MAL_HSPEC_I2C_SIZE = 2
} mal_hspec_i2c_e;

typedef struct {
	mal_hspec_i2c_e interface;
	const mal_hspec_gpio_s *scl_gpio;
	const mal_hspec_gpio_s *sda_gpio;
	uint64_t bitrate;
} mal_hspec_i2c_init_s;

typedef enum {
	MAL_HSPEC_I2C_READ,
	MAL_HSPEC_I2C_WRITE
} mal_hspec_i2c_cmd_e;

typedef struct {
	volatile uint8_t address;
	volatile uint8_t *buffer;
	volatile uint8_t packet_size;
	mal_hspec_i2c_cmd_e cmd;
} mal_hspec_i2c_packet_s;

typedef enum {
	MAL_HSPEC_I2C_SUCCESS,
	MAL_HSPEC_I2C_NACK_COMPLETE,
	MAL_HSPEC_I2C_NACK_INCOMPLETE,
	MAL_HSPEC_I2C_BUS_ERROR
} mal_hspec_i2c_result_e;

typedef struct MAL_HSPEC_I2C_MSG mal_hspec_i2c_msg_s;

typedef mal_error_e (*mal_hspec_i2c_callback_t)(mal_hspec_i2c_e interface, mal_hspec_i2c_packet_s *packet, mal_hspec_i2c_result_e result, mal_hspec_i2c_msg_s **next_msg);

typedef struct MAL_HSPEC_I2C_MSG {
	mal_hspec_i2c_packet_s packet;
	mal_hspec_i2c_callback_t callback;
} mal_hspec_i2c_msg_s;

// CAN

#define MAL_HSPEC_CAN_MAX_DATA_SIZE	8

typedef enum {
	MAL_HSPEC_CAN_1 = 0,
	MAL_HSPEC_CAN_2 = 1,
	MAL_HSPEC_CAN_SIZE = 2
} mal_hspec_can_e;

typedef enum {
	MAL_HSPEC_CAN_ID_STANDARD,
	MAL_HSPEC_CAN_ID_EXTENDED
} mal_hspec_can_id_type_e;

typedef struct {
	uint8_t data[MAL_HSPEC_CAN_MAX_DATA_SIZE];
	uint8_t size;
	uint32_t id;
	mal_hspec_can_id_type_e id_type;
} mal_hspec_can_msg_s;

typedef mal_error_e (*mal_hspec_can_tx_callback_t)(mal_hspec_can_e interface, mal_hspec_can_msg_s *next_msg);

typedef mal_error_e (*mal_hspec_can_rx_callback_t)(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg);

typedef struct {
	mal_hspec_can_e interface;
	mal_hspec_gpio_s *tx_gpio;
	mal_hspec_gpio_s *rx_gpio;
	uint64_t bitrate;
	mal_hspec_can_tx_callback_t tx_callback;
	mal_hspec_can_rx_callback_t rx_callback;
} mal_hspec_can_init_s;

typedef struct {
	uint32_t id;
	uint32_t mask;
	mal_hspec_can_id_type_e id_type;
} mal_hspec_can_filter_s;

// ADC

typedef enum {
	MAL_HSPEC_ADC_0 = 0,
	MAL_HSPEC_ADC_1 = 1,
	MAL_HSPEC_ADC_2 = 2,
	MAL_HSPEC_ADC_3 = 3,
	MAL_HSPEC_ADC_4 = 4,
	MAL_HSPEC_ADC_5 = 5,
	MAL_HSPEC_ADC_6 = 6,
	MAL_HSPEC_ADC_7 = 7,
	MAL_HSPEC_ADC_8 = 8,
	MAL_HSPEC_ADC_9 = 9,
	MAL_HSPEC_ADC_10 = 10,
	MAL_HSPEC_ADC_11 = 11,
	MAL_HSPEC_ADC_12 = 12,
	MAL_HSPEC_ADC_13 = 13,
	MAL_HSPEC_ADC_14 = 14,
	MAL_HSPEC_ADC_15 = 15,
	MAL_HSPEC_ADC_SIZE = 16
} mal_hspec_adc_e;

typedef struct {
	mal_hspec_adc_e adc;
	const mal_hspec_gpio_s *gpio;
	uint8_t bit_resolution;
} mal_hspec_adc_init_s;

typedef mal_error_e (*mal_hspec_adc_read_callback_t)(mal_hspec_adc_e adc, uint64_t value);

#endif /* HSPEC_MAL_HSPEC_CMN_H_ */
