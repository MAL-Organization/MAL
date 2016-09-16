/**
 * @file mal_flash.h
 * @author Olivier Allaire
 * @date September 2 2016
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
 * @brief Functions to write and erase the flash.
 */

#ifndef FLASH_MAL_FLASH_H_
#define FLASH_MAL_FLASH_H_

#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "hspec/mal_hspec.h"

/**
 * @defgroup CAN
 * @brief @copybrief mal_flash.h
 * @{
 */

/**
 * @brief Read an unsigned 8 bit integer from memory.
 * @param address The memory address to read from.
 */
#define mal_flash_read_uint8(address) mal_hspec_flash_read_uint8(address)

/**
 * @brief Read a signed 8 bit integer from memory.
 * @param address The memory address to read from.
 */
#define mal_flash_read_int8(address) mal_hspec_flash_read_int8(address)

/**
 * @brief Read an unsigned 16 bit integer from memory.
 * @param address The memory address to read from.
 */
#define mal_flash_read_uint16(address) mal_hspec_flash_read_uint16(address)

/**
 * @brief Read a signed 16 bit integer from memory.
 * @param address The memory address to read from.
 */
#define mal_flash_read_int16(address) mal_hspec_flash_read_int16(address)

/**
 * @brief Read an unsigned 32 bit integer from memory.
 * @param address The memory address to read from.
 */
#define mal_flash_read_uint32(address) mal_hspec_flash_read_uint32(address)

/**
 * @brief Read a signed 32 bit integer from memory.
 * @param address The memory address to read from.
 */
#define mal_flash_read_int32(address) mal_hspec_flash_read_int32(address)

/**
 * @brief Read an unsigned 64 bit integer from memory.
 * @param address The memory address to read from.
 */
#define mal_flash_read_uint64(address) mal_hspec_flash_read_uint64(address)

/**
 * @brief Read a signed 64 bit integer from memory.
 * @param address The memory address to read from.
 */
#define mal_flash_read_int64(address) mal_hspec_flash_read_int64(address)

/**
 * @brief This function will write an array of uint16 values to flash from the
 * given start address.
 * @param start_address The start address to write. Take care to give a valid
 * address for the running MCU.
 * @param values A pointer to the array of values to write.
 * @param count The count of values in the values array.
 * @return Returns #MAL_ERROR_OK on success.
 */
#define mal_flash_write_uint16_values(start_address, values, count) mal_hspec_flash_write_uint16_values(start_address, values, count)

/**
 * @brief This function will write an array of uint32 values to flash from the
 * given start address.
 * @param start_address The start address to write. Take care to give a valid
 * address for the running MCU.
 * @param values A pointer to the array of values to write.
 * @param count The count of values in the values array.
 * @return Returns #MAL_ERROR_OK on success.
 */
#define mal_flash_write_uint32_values(start_address, values, count) mal_hspec_flash_write_uint32_values(start_address, values, count)

/**
 * @brief This function will return the start address of a flash page.
 * @param page The page number. Take care to give a valid page number for the
 * running MCU.
 * @return Returns a uint64_t with the address.
 */
#define mal_flash_get_page_start_address(page) mal_hspec_flash_get_page_start_address(page)

/**
 * @brief This function will return the page which contains the given address.
 * @param address A uint64_t address.
 * @return Returns the page as a uint32_t.
 */
#define mal_flash_get_page_from_address(address) mal_hspec_flash_get_page_from_address(address)

/**
 * @brief This function will return the size of a flash page.
 * @param page The page number. Take care to give a valid page number for the
 * running MCU.
 * @return Returns a uint32_t with the address.
 */
#define mal_flash_get_page_size(page) mal_hspec_flash_get_page_size(page)

/**
 * This function will erase flash page.
 * @param page The page number. Take care to give a valid page number for the
 * running MCU.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_flash_erase_page(uint32_t page);

/**
 * @}
 */

#endif /* FLASH_MAL_FLASH_H_ */
