/*
 * Copyright (c) 2018 Olivier Allaire
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

#include "mal_hspec_stm32f7_flash.h"
#include "flash/mal_flash.h"
#include "stm32f7/stm32f7xx_hal_flash.h"
#include "power/mal_power.h"

#define FLASH_START_ADDRESS	    0x08000000

mal_error_e mal_flash_erase_page(uint32_t page) {
    mal_error_e mal_result;
    HAL_StatusTypeDef hal_result;
    // Get operating voltage
    mal_volts_t vdd;
    mal_result = mal_power_get_rail_voltage(MAL_POWER_RAIL_VDD, &vdd);
    if (MAL_ERROR_OK != mal_result) {
        return mal_result;
    }
    uint64_t mv_vdd = MAL_TYPES_MAL_VOLTS_TO_MILLIVOLTS(vdd);
    // Unlock flash registers
    HAL_FLASH_Unlock();
    // Erase page
    uint32_t sector_error;
    FLASH_EraseInitTypeDef erase_init;
    erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
    if (mv_vdd >= 2700) {
        erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    } else if (mv_vdd >= 2100) {
        erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_2;
    } else {
        erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_1;
    }
    erase_init.Sector = mal_hspec_stm32f7_flash_get_sector(page);
    erase_init.NbSectors = 1;
    hal_result = HAL_FLASHEx_Erase(&erase_init, &sector_error);
    // Lock flash
    HAL_FLASH_Lock();
    if (HAL_OK != hal_result) {
        return MAL_ERROR_MEMORY;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_flash_write_uint16_values(unsigned int start_address, uint16_t *values, uint32_t count) {
    HAL_StatusTypeDef hal_result = HAL_OK;
    // Unlock flash registers
    HAL_FLASH_Unlock();
    // Write values
    uint32_t i;
    uint32_t address = start_address;
    for (i = 0; i < count; i++) {
        // Write data
        hal_result = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, values[i]);
        if (HAL_OK != hal_result) {
            // We stop at first sign of error
            break;
        }
        // Increment address
        address += sizeof(uint16_t);
    }
    // Lock flash registers
    HAL_FLASH_Lock();
    // Return result
    if (HAL_OK != hal_result) {
        return MAL_ERROR_MEMORY;
    }
    return MAL_ERROR_OK;
}

mal_error_e mal_flash_write_uint32_values(unsigned int start_address, uint32_t *values, uint32_t count) {
    HAL_StatusTypeDef hal_result = HAL_OK;
    // Unlock flash registers
    HAL_FLASH_Unlock();
    // Write values
    uint32_t i;
    uint32_t address = start_address;
    for (i = 0; i < count; i++) {
        // Write data
        hal_result = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, values[i]);
        if (HAL_OK != hal_result) {
            // We stop at first sign of error
            break;
        }
        // Increment address
        address += sizeof(uint32_t);
    }
    // Lock flash registers
    HAL_FLASH_Lock();
    // Return result
    if (HAL_OK != hal_result) {
        return MAL_ERROR_MEMORY;
    }
    return MAL_ERROR_OK;
}

unsigned int mal_flash_get_page_start_address(uint32_t page) {
    unsigned int address = FLASH_START_ADDRESS;
    // Add space for previous pages
    uint32_t i;
    for (i = 0; i < page; i++) {
        address += mal_flash_get_page_size(i);
    }
    return address;
}

uint32_t mal_flash_get_page_from_address(unsigned int address) {
    // Check address
    if (address < FLASH_START_ADDRESS) {
        return 0;
    }
    // Browse pages
    uint32_t page_start_address = FLASH_START_ADDRESS;
    uint32_t page_count = mal_flash_get_page_count();
    uint32_t page;
    for (page = 0; page < page_count; page++) {
        // Get end address
        uint32_t page_size = mal_flash_get_page_size(page);
        uint32_t page_end_address = page_start_address + page_size - 1;
        // Check if address is within the range
        if (address >= page_start_address && address <= page_end_address) {
            return page;
        }
        // Prepare start address for next page
        page_start_address += page_size;
    }
    return page_count - 1;
}

MAL_DEFS_INLINE uint8_t mal_flash_read_uint8(unsigned int address) {
    return *((uint8_t*)(int)(address));
}

MAL_DEFS_INLINE int8_t mal_flash_read_int8(unsigned int address) {
    return *((int8_t*)(int)(address));
}

MAL_DEFS_INLINE uint16_t mal_flash_read_uint16(unsigned int address) {
    return *((uint16_t*)(int)(address));
}

MAL_DEFS_INLINE int16_t mal_flash_read_int16(unsigned int address) {
    return *((int16_t*)(int)(address));
}

MAL_DEFS_INLINE uint32_t mal_flash_read_uint32(unsigned int address) {
    return *((uint32_t*)(int)(address));
}

MAL_DEFS_INLINE int32_t mal_flash_read_int32(unsigned int address) {
    return *((int32_t*)(int)(address));
}

MAL_DEFS_INLINE uint64_t mal_flash_read_uint64(unsigned int address) {
    return *((uint64_t*)(int)(address));
}

MAL_DEFS_INLINE int64_t mal_flash_read_int64(unsigned int address) {
    return *((int64_t*)(int)(address));
}

bool mal_hspec_stm32f7_flash_is_dual_boot(void) {
    FLASH_OBProgramInitTypeDef options;
    HAL_FLASH_OB_Unlock();
    HAL_FLASHEx_OBGetConfig(&options);
    HAL_FLASH_OB_Lock();
    return  (options.USERConfig & OB_NDBANK_SINGLE_BANK) == OB_NDBANK_DUAL_BANK;
}
