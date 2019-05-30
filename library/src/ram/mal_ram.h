/**
 * @file mal_ram.h
 * @author Olivier Allaire
 * @date Mai 30 2019
 * @copyright Copyright (c) 2019 Olivier Allaire
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
 * @brief Functions to get state of RAM.
 */

#ifndef MAL_MAL_RAM_H
#define MAL_MAL_RAM_H

#include "std/mal_types.h"
#include "std/mal_defs.h"
#include "std/mal_stdint.h"

/**
 * @defgroup RAM
 * @brief @copybrief mal_ram.h
 * @{
 */

/**
 * This value is used to keep trace of which stack addresses were accessed.
 */
#define MAL_RAM_STACK_MARKER        0x42

/**
 * The stack will be considered untouched if MAL_RAM_STACK_STOP_COUNT markers in row are found.
 */
#define MAL_RAM_STACK_STOP_COUNT    5

/**
 * @brief This function allows other RAM stack functions to function properly.
 */
void mal_ram_init_stack_monitor(void);

/**
 * Provides the RAM stack size.
 * @return The RAM stack size.
 */
unsigned int mal_ram_get_stack_size(void);

/**
 * @brief For this function to work properly, mal_ram_init_stack_monitor must be called once before. This will return a
 * ratio of the total stack size used.
 * @return The ratio of the total stack used. 0 Means no stack was used, full ratio means the stack is full.
 */
mal_ratio_t mal_ram_get_stack_usage(void);

/**
 * @brief Returns the start of the stack. To prevent confusion, the start is the lowest address of the stack. For
 * example, if the stack goes from 80 to 199, this function will return 80.
 * @return The start in RAM memory of the stack.
 */
unsigned int mal_ram_get_stack_start_address(void);

/**
 * @brief Returns the end of the stack. To prevent confusion, the end is the highest address of the stack. For example,
 * if the stack goes from 80 to 199, this function will return 199.
 * @return The start in RAM memory of the stack.
 */
unsigned int mal_ram_get_stack_end_address(void);

/**
 * @brief This function returns the current stack pointer of the processor.
 * @return The current stack pointer of the processor.
 */
unsigned int mal_ram_get_stack_pointer_address(void);

/**
 * @brief Read from an address in RAM. This is mostly used to allow unit tests of MAL RAM functions.
 * @param address The address to read from.
 * @return The value read.
 */
MAL_DEFS_INLINE uint8_t mal_ram_read(unsigned int address);

/**
 * @brief Write to an address in RAM. This is mostly used to allow unit tests of MAL RAM functions.
 * @param address The address to write to.
 * @param value The value to write.
 */
MAL_DEFS_INLINE void mal_ram_write(unsigned int address, uint8_t value);

/**
 * @}
 */

#endif //MAL_MAL_RAM_H
