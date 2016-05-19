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
 * @brief Initialize an SPI interface as a master interface.
 * @param init The initialization parameters.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_spi_init_master(mal_hspec_spi_master_init_s *init);

/**
 * @}
 */

#endif /* SPI_MAL_SPI_H_ */
