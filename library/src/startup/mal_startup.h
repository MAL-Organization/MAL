/**
 * @file mal_startup.h
 * @author Olivier Allaire
 * @date May 16 2015
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
 * @brief MCU startup functions.
 */

#ifndef STARTUP_MAL_STARTUP_H_
#define STARTUP_MAL_STARTUP_H_

/**
 * @defgroup STARTUP
 * @brief @copybrief mal_startup.h
 * @{
 */

/**
 * @brief Entry point of the code. Will eventually end up in the main.
 */
void mal_startup(void);

/**
 * @brief Where the actual initialization takes place. Useful for unit testing,
 * do not call directly otherwise.
 */
void mal_startup_top_level(void);

/**
 * @brief MCU specific startup.
 */
void mal_startup_hardware(void);

/**
 * @}
 */

#endif /* STARTUP_MAL_STARTUP_H_ */
