/**
 * @file mal_reset.h
 * @author Olivier Allaire
 * @date July 27 2016
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
 * @brief Functions to handle the reset of the MCU from user code.
 */

#ifndef RESET_MAL_RESET_H_
#define RESET_MAL_RESET_H_

/**
 * @defgroup RESET
 * @brief @copybrief mal_reset.h
 * @{
 */

#include "hspec/mal_hspec.h"

/**
 * Calling this will reset the MCU.
 */
#define mal_reset_mcu() mal_hspec_reset_mcu()

/**
 * Return the last reset source.
 * @return Return the last reset source.
 */
mal_hspec_reset_source_e mal_reset_get_source(void);

/**
 * @}
 */

#endif /* RESET_MAL_RESET_H_ */
