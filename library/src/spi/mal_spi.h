/**
 * @file mal_spi.h
 * @author Olivier Allaire
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
 * @brief Functions to access and configure the SPI interfaces of the MCU.
 */

#ifndef SPI_MAL_SPI_H_
#define SPI_MAL_SPI_H_

#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "gpio/mal_gpio_definitions.h"
#include "std/mal_defs.h"
#include "std/mal_types.h"

/**
 * @defgroup SPI
 * @brief @copybrief mal_spi.h
 * @{
 */

/**
* SPI handle that must be defined by hardware specific implementation. Used
* to access the SPI functions.
*/
typedef struct MAL_SPI mal_spi_s;

/**
 * The possible SPI interfaces.
 */
typedef enum {
    MAL_SPI_1 = 0,  //!< SPI interface 1
    MAL_SPI_2 = 1,  //!< SPI interface 2
    MAL_SPI_SIZE = 2//!< Number of possible SPI interfaces
} mal_spi_e;

/**
 * Possible modes for the select pin of an interface.
 */
typedef enum {
    MAL_SPI_SELECT_MODE_HARDWARE,//!< The select pin is controlled by hardware.
    MAL_SPI_SELECT_MODE_SOFTWARE,//!< The select pin is controlled by software by the driver.
    MAL_SPI_SELECT_MODE_USER,    //!< The select pin is controlled by software by the user code.
    MAL_SPI_SELECT_MODE_NONE     //!< The select pin is not controlled at all.
} mal_spi_select_mode_e;

/**
 * The possible word sizes for data transactions
 */
typedef enum {
    MAL_SPI_DATA_4_BITS = 4,  //!< MAL_SPI_DATA_4_BITS
    MAL_SPI_DATA_5_BITS = 5,  //!< MAL_SPI_DATA_5_BITS
    MAL_SPI_DATA_6_BITS = 6,  //!< MAL_SPI_DATA_6_BITS
    MAL_SPI_DATA_7_BITS = 7,  //!< MAL_SPI_DATA_7_BITS
    MAL_SPI_DATA_8_BITS = 8,  //!< MAL_SPI_DATA_8_BITS
    MAL_SPI_DATA_9_BITS = 9,  //!< MAL_SPI_DATA_9_BITS
    MAL_SPI_DATA_10_BITS = 10,//!< MAL_SPI_DATA_10_BITS
    MAL_SPI_DATA_11_BITS = 11,//!< MAL_SPI_DATA_11_BITS
    MAL_SPI_DATA_12_BITS = 12,//!< MAL_SPI_DATA_12_BITS
    MAL_SPI_DATA_13_BITS = 13,//!< MAL_SPI_DATA_13_BITS
    MAL_SPI_DATA_14_BITS = 14,//!< MAL_SPI_DATA_14_BITS
    MAL_SPI_DATA_15_BITS = 15,//!< MAL_SPI_DATA_15_BITS
    MAL_SPI_DATA_16_BITS = 16 //!< MAL_SPI_DATA_16_BITS
} mal_spi_data_size_e;

/**
 * Possible states for clock when there is no active transactions.
 */
typedef enum {
    MAL_SPI_CLK_IDLE_STATE_LOW,//!< MAL_HSPEC_SPI_CLK_IDLE_STATE_LOW
    MAL_SPI_CLK_IDLE_STATE_HIGH//!< MAL_HSPEC_SPI_CLK_IDLE_STATE_HIGH
} mal_spi_clk_idle_state_e;

/**
 * Possible values for valid data on the bus. The edge refers to the clock
 * signal.
 */
typedef enum {
    MAL_SPI_DATA_LATCH_EDGE_RISING,//!< MAL_SPI_DATA_LATCH_EDGE_RISING
    MAL_SPI_DATA_LATCH_EDGE_FALLING//!< MAL_SPI_DATA_LATCH_EDGE_FALLING
} mal_spi_data_latch_edge_e;

/**
 * Possible values of the select IO when selecting the device.
 */
typedef enum {
    MAL_SPI_SELECT_POLARITY_LOW,//!< The select is active low.
    MAL_SPI_SELECT_POLARITY_HIGH//!< The select is active high.
} mal_spi_select_polarity_e;

/**
 * Possible values for bit ordering.
 */
typedef enum {
    MAL_SPI_BIT_ORDER_MSB,//!< Most significant bit first
    MAL_SPI_BIT_ORDER_LSB //!< Least significant bit first
} mal_spi_bit_order_e;

/**
 * All parameters to initialize an SPI interface as a master.
 */
typedef struct {
    mal_spi_e interface;/**< The SPI interface to initialize.*/
    mal_hertz_t clock_speed;/**< The clock speed to set.*/
    mal_gpio_port_e mosi_port; /**< The port of the MOSI GPIO to initialize.*/
    uint8_t mosi_pin; /**< The pin of the port of the MOSI GPIO to initialize. */
    mal_gpio_port_e miso_port; /**< The port of the MISO GPIO to initialize.*/
    uint8_t miso_pin; /**< The pin of the port of the MISO GPIO to initialize. */
    mal_gpio_port_e clk_port; /**< The port of the CLK GPIO to initialize.*/
    uint8_t clk_pin; /**< The pin of the port of the CLK GPIO to initialize. */
    mal_gpio_port_e select_port; /**< The port of the select GPIO to initialize. This can be omitted. This could be because the mode is user or there is no global select pin.*/
    uint8_t select_pin; /**< The pin of the port of the select GPIO to initialize. This can be omitted. This could be because the mode is user or there is no global select pin.*/
    mal_spi_select_mode_e select_mode;/**< The select mode.*/
    mal_spi_data_size_e data_size;/**< The size of each words.*/
    mal_spi_bit_order_e bit_order;/**< The order of each bit in a word.*/
    mal_spi_clk_idle_state_e clk_idle_state;/**< Clock idle state.*/
    mal_spi_data_latch_edge_e latch_edge;/**< Data latch on clock edge.*/
    mal_spi_select_polarity_e select_polarity;/**< Select pin polarity.*/
} mal_spi_master_init_s;

/// @cond SKIP
typedef struct MAL_SPI_MSG mal_spi_msg_s;
/// @endcond

/**
 * This callback will be executed at the end of a transaction where the MCU is
 * the master.
 * @param handle The handle given.
 * @param msg The message of the transaction containing only the received data.
 * @param next_msg The next message to send. Set to #NULL if no message should
 * be transmitted.
 */
typedef void (*mal_spi_master_transaction_complete_t)(void *handle, mal_spi_msg_s *msg, mal_spi_msg_s **next_msg);

/**
 * This structures contains all the data to do an SPI transaction.
 */
typedef struct MAL_SPI_MSG {
    mal_gpio_s *select;/**< The select pin for this message. If no pins
                                  should be used (such as the default pin was
                                  set by the initialization), set this to
                                  #NULL.*/
    mal_spi_select_polarity_e select_polarity;/**< The polarity of the
                                                         select pin. If select
                                                         is #NULL, this isn't
                                                         taken into account.*/
    uint16_t *data;/**< A pointer to the data buffer to send and receive.*/
    uint8_t data_length;/**< The length of the transaction in words.*/
    mal_spi_master_transaction_complete_t callback;/**< The callback to
                                                              execute when the
                                                              transaction is
                                                              over.*/
    void *handle; /**< This handle will be passed to the callback.*/
} mal_spi_msg_s;

/**
 * @brief Start an SPI transfer as a master. Note that this is not a blocking
 * call. Use the callback of the message to receive the result.
 * @param handle The SPI interface to use.
 * @param msg A pointer to the SPI message of type ::mal_hspec_spi_msg_s. Note
 * that this pointer should point to static memory space. The driver will use
 * the message during the transfer.
 * @return @MAL_ERROR_OK on success.
 */
mal_error_e mal_spi_start_transaction(mal_spi_s *handle, mal_spi_msg_s *msg);

/**
 * @brief Disable an SPI interrupt.
 * @param handle The interface to disable the interrupt from.
 * @return Returns true if interrupt was active before disabling it.
 */
MAL_DEFS_INLINE bool mal_spi_disable_interrupt(mal_spi_s *handle);

/**
 * @brief Set an SPI interrupt.
 * @param handle The interface to set the interrupt from.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 */
MAL_DEFS_INLINE void mal_spi_set_interrupt(mal_spi_s *handle, bool active);

/**
 * @brief Initialize an SPI interface as a master interface.
 * @param init The initialization parameters.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_spi_init_master(mal_spi_master_init_s *init, mal_spi_s *handle);

/**
 * This include is last because it defines hardware specific implementations of
 * structures. If not included last, circular dependencies will arise.
 */
#include "hspec/mal_hspec.h"

/**
 * @}
 */

#endif /* SPI_MAL_SPI_H_ */
