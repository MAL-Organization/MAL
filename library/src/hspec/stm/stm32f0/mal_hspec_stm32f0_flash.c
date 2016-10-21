/*
 * mal_hspec_stm32f0_flash.c
 *
 *  Created on: Sep 2, 2016
 *      Author: Olivier
 */
/*
 * Copyright (c) 2015 Olivier Allaire
 *
 * This file is part of MAL.
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
 */

#include "mal_hspec_stm32f0_can.h"
#include "stm32f0/stm32f0xx_flash.h"
#include "mal_hspec_stm32f0_cmn.h"

#define FLASH_START_ADDRESS	0x08000000

mal_error_e mal_hspec_stm32f0_flash_erase_page(uint32_t page) {
	// Unlock flash registers
	FLASH_Unlock();
	// Clear pending flags
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	// Erase page
	FLASH_Status result;
	uint32_t offset = mal_hspec_stm32f0_flash_get_page_size(page) * page;
	result = FLASH_ErasePage(FLASH_START_ADDRESS + offset);
	// Lock flash registers
	FLASH_Lock();
	// Return result
	if (FLASH_COMPLETE != result) {
		return MAL_ERROR_MEMORY;
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_flash_write_uint16_values(uint64_t start_address, uint16_t *values, uint32_t count) {
	FLASH_Status result;
	// Unlock flash registers
	FLASH_Unlock();
	// Clear pending flags
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	// Write values
	uint32_t i;
	uint32_t address = start_address;
	for (i = 0; i < count; i++) {
		// Write data
		result = FLASH_ProgramHalfWord(address, values[i]);
		if (FLASH_COMPLETE != result) {
			// We stop at first sign of error
			break;
		}
		// Increment address
		address += sizeof(uint16_t);
	}
	// Lock flash registers
	FLASH_Lock();
	// Return result
	if (FLASH_COMPLETE != result) {
		return MAL_ERROR_MEMORY;
	}
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm32f0_flash_write_uint32_values(uint64_t start_address, uint32_t *values, uint32_t count) {
	FLASH_Status result;
	// Unlock flash registers
	FLASH_Unlock();
	// Clear pending flags
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
	// Write values
	uint32_t i;
	uint32_t address = start_address;
	for (i = 0; i < count; i++) {
		// Write data
		result = FLASH_ProgramWord(address, values[i]);
		if (FLASH_COMPLETE != result) {
			// We stop at first sign of error
			break;
		}
		// Increment address
		address += sizeof(uint32_t);
	}
	// Lock flash registers
	FLASH_Lock();
	// Return result
	if (FLASH_COMPLETE != result) {
		return MAL_ERROR_MEMORY;
	}
	return MAL_ERROR_OK;
}

uint64_t mal_hspec_stm32f0_flash_get_page_start_address(uint32_t page) {
	uint32_t page_size = mal_hspec_stm32f0_flash_get_page_size(page);
	uint64_t address = FLASH_START_ADDRESS + (page_size * page);
	return address;
}

uint32_t mal_hspec_stm32f0_flash_get_page_from_address(uint64_t address) {
	// Check address
	if (address < FLASH_START_ADDRESS) {
		return 0;
	}
	// Get page
	uint32_t page_size = mal_hspec_stm32f0_flash_get_page_size(0);
	uint32_t page = (address - FLASH_START_ADDRESS) / page_size;

	return page;
}
