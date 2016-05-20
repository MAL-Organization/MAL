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
#include "hspec/mal_hspec.h"

/**
 * @defgroup SPI
 * @brief @copybrief mal_spi.h
 * @{
 */

/**
 * @brief Start an SPI transfer as a master. Note that this is not a blocking
 * call. Use the callback of the message to receive the result.
 * @param interface The SPI interface from ::mal_hspec_spi_e.
 * @param msg A pointer to the SPI message of type ::mal_hspec_spi_msg_s. Note
 * that this pointer should point to static memory space. The driver will use
 * the message during the transfer.
 * @return @MAL_ERROR_OK on success.
 */
#define mal_spi_start_transaction(interface, msg) mal_hspec_spi_start_transaction(interface, msg)

/**
 * @brief Disable an SPI interrupt.
 * @param interface The interface to disable the interrupt from of type
 * ::mal_hspec_spi_e.
 * @return Returns true if interrupt was active before disabling it.
 */
#define mal_spi_disable_interrupt(interface) mal_hspec_spi_disable_interrupt(interface)

/**
 * @brief Enable an SPI interrupt.
 * @param interface The interface to enable the interrupt from of type
 * ::mal_hspec_spi_e.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
#define mal_spi_enable_interrupt(interface, active) mal_hspec_spi_enable_interrupt(interface, active)

/**
 * @brief Initialize an SPI interface as a master interface.
 * @param init The initialization parameters.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_spi_init_master(mal_hspec_spi_master_init_s *init);

/**
 * @}
 */

#endif /* SPI_MAL_SPI_H_ */
