/**
 * @file mal_i2c.h
 * @author Olivier Allaire
 * @date June 1 2015
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
 * @brief Functions to access and configure the I2C interfaces of the MCU.
 */

#ifndef I2C_MAL_I2C_H_
#define I2C_MAL_I2C_H_

#include "gpio/mal_gpio_definitions.h"
#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "std/mal_defs.h"
#include "std/mal_bool.h"

/**
 * @defgroup I2C
 * @brief @copybrief mal_i2c.h
 * @{
 */

/**
* CAN handle that must be defined by hardware specific implementation. Used
* to access the CAN functions.
*/
typedef struct MAL_I2C mal_i2c_s;

/**
 * Possible I2C interfaces.
 */
typedef enum {
    MAL_I2C_1 = 0,  //!< I2C interface 1
    MAL_I2C_2 = 1,  //!< I2C interface 2
    MAL_I2C_SIZE = 2//!< Number of possible I2C interfaces
} mal_i2c_e;

/**
 * I2C initialization parameters.
 */
typedef struct {
    mal_i2c_e interface; /**< The I2C interface.*/
    mal_gpio_port_e scl_port; /**< The port of the SCL GPIO to initialize.*/
    uint8_t scl_pin; /**< The pin of the port of the SCL GPIO to initialize. */
    mal_gpio_port_e sda_port; /**< The port of the SDA GPIO to initialize.*/
    uint8_t sda_pin; /**< The pin of the port of the SDA GPIO to initialize. */
    uint64_t bitrate; /**< The bitrate of the interface.*/
} mal_i2c_init_s;

/**
 * The possible command types.
 */
typedef enum {
    MAL_I2C_READ,//!< MAL_HSPEC_I2C_READ
    MAL_I2C_WRITE//!< MAL_HSPEC_I2C_WRITE
} mal_i2c_cmd_e;

/**
 * This structure defines the core part of an I2C message.
 */
typedef struct {
    volatile uint8_t address; /**< The address of the transaction.*/
    volatile uint8_t *buffer; /**< The buffer containing the data for the transaction.*/
    volatile uint8_t packet_size; /**< The number of words in the buffer.*/
    mal_i2c_cmd_e cmd; /**< The type of transactions.*/
} mal_i2c_packet_s;

/**
 * Possible I2C transactions results.
 */
typedef enum {
    MAL_I2C_SUCCESS,        //!< Successful transaction.
    MAL_I2C_NACK_COMPLETE,  //!< Transaction completed, but device did not acknowledge.
    MAL_I2C_NACK_INCOMPLETE,//!< Transaction is incomplete, and the device did not acknowledge.
    MAL_I2C_BUS_ERROR       //!< An error occurred on the bus during the transaction.
} mal_i2c_result_e;

/// @cond SKIP
typedef struct MAL_I2C_MSG mal_i2c_msg_s;
/// @endcond

/**
 * @brief This callback is used when a transaction completes.
 * @param handle The handle given with the message;
 * @param packet The packet received.
 * @param result The result of the transaction.
 * @param next_msg The next message to send. This message will be sent immediately. If there is no message to send,
 * set this to #NULL.
 */
typedef void (*mal_i2c_callback_t)(void *handle, mal_i2c_packet_s *packet, mal_i2c_result_e result,
                                   mal_i2c_msg_s **next_msg);

/**
 * I2C message.
 */
typedef struct /** @cond SKIP*/MAL_I2C_MSG /** @endcond*/ {
    mal_i2c_packet_s packet; /**< The packet containing the core of the message.*/
    mal_i2c_callback_t callback; /**< The callback to execute once the transaction is complete.*/
    void *handle; /**< This will be passed to the callback.*/
} mal_i2c_msg_s;

/**
 * @brief Initialize an I2C interface as a master interface.
 * @param init The initialization parameters.
 * @param handle The handle to initialize. This handle is used to access
 * subsequent functions.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_i2c_init_master(mal_i2c_init_s *init, mal_i2c_s *handle);

/**
 * @brief Function to initialize directly the interface. Using this function
 * will reduce code size at the cost of flexibility and safety. Usually use
 * this in a code closer to final version.
 * @param init Initialization parameters.
 * @param direc_init A pointer to direct initialization parameter. See the
 * hardware specific implementation to know what type this should be.
 * @param handle The handle to initialize. This handle is used to access
 * subsequent functions.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_i2c_master_direct_init(mal_i2c_init_s *init, const void *direct_init, mal_i2c_s *handle);

/**
 * @brief Start an I2C transfer as a master. Note that this is not a blocking
 * call. Use the callback of the message to receive the result.
 * @param handle The I2C handle to start the transfer on.
 * @param msg A pointer to the I2C message to send.
 * @return #MAL_ERROR_OK on success.
 */
mal_error_e mal_i2c_transfer(mal_i2c_s *handle, mal_i2c_msg_s *msg);

/**
 * @brief Disable an I2C interrupt.
 * @param handle The interface to disable the interrupt from.
 * @return Returns true if interrupt was active before disabling it.
 */
MAL_DEFS_INLINE bool mal_i2c_disable_interrupt(mal_i2c_s *handle);

/**
 * @brief Enable an I2C interrupt.
 * @param handle The interface to enable the interrupt from.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 */
MAL_DEFS_INLINE void mal_i2c_set_interrupt(mal_i2c_s *handle, bool active);

/**
 * This include is last because it defines hardware specific implementations of
 * structures. If not included last, circular dependencies will arise.
 */
#include "hspec/mal_hspec.h"

/**
 * @}
 */

#endif /* I2C_MAL_I2C_H_ */
