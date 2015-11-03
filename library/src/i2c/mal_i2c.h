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

#include "gpio/mal_gpio.h"
#include "std/mal_error.h"
#include "hspec/mal_hspec.h"

/**
 * @defgroup I2C
 * @brief @copybrief mal_i2c.h
 * @{
 */

/**
 * @brief Start an I2C transfer as a master. Note that this is not a blocking
 * call. Use the callback of the message to receive the result.
 * @param interface The I2C interface from ::mal_hspec_i2c_e.
 * @param msg A pointer to the I2C message of type ::mal_hspec_i2c_msg_s.
 * @return @MAL_ERROR_OK on success.
 */
#define mal_i2c_transfer(interface, msg) mal_hspec_i2c_transfer(interface, msg)

/**
 * @brief Disable an I2C interrupt.
 * @param interface The interface to disable the interrupt from of type
 * ::mal_hspec_i2c_e.
 * @return Returns true if interrupt was active before disabling it.
 */
#define mal_i2c_disable_interrupt(interface) mal_hspec_i2c_disable_interrupt(interface)

/**
 * @brief Enable an I2C interrupt.
 * @param interface The interface to enable the interrupt from of type
 * ::mal_hspec_i2c_e.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
#define mal_i2c_enable_interrupt(interface, active) mal_hspec_i2c_enable_interrupt(interface, active)

/**
 * @brief Initialize an I2C interface as a master interface.
 * @param init The initialization parameters.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_i2c_init_master(mal_hspec_i2c_init_s *init);

/**
 * @}
 */

#endif /* I2C_MAL_I2C_H_ */
