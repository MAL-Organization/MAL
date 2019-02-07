/**
 * @file mal_e3prom.h
 * @author Olivier Allaire
 * @date January 14 2019
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
 * @brief Emulated electrically erasable programmable read-only memory utility.
 * This is emulated because flash memory can only be erased in sections and only
 * written to once per address before erasing. To accomplish this emulation, we
 * need at least 2 flash pages. When 1 section is full, it will switch section
 * and so on. It is left to the user to decide of the size (number of pages) of
 * each section. This is for 2 factors. The first is each MCU has a different
 * flash setup. The other depends on how many variables the user wishes to
 * store. This part is important. In this implementation, it is up to the user
 * to reserve a possible number of keys. If the sections are not big enough,
 * the e3prom will overflow and simply fail. To know how much data you can
 * store, use the following equation:
 * sector_size_in_kB / 8 - 3
 * For a section of 2kB, it would be:
 * 2048 / 8 - 3 = 253 different keys. More than this and it would overflow and
 * fail.
 * @note Emulated eeprom is the most ridiculous and incongruous acronym ever.
 * It's just more than 30 years of just adding adjectives to adapt to
 * technology, seriously!
 */

#ifndef UTILS_MAL_E3PROM_H_
#define UTILS_MAL_E3PROM_H_

#include "std/mal_stdint.h"
#include "std/mal_error.h"
#include "std/mal_bool.h"

/**
 * This address is reserved for the state of sections of e3prom.
 */
#define MAL_E3PROM_STATE_KEY	0xFFFFFFFE

/**
 * These are the initialization parameters of the e3prom.
 */
typedef struct {
	uint32_t primary_start_page;	//!< The primary section start flash page.
	uint32_t primary_page_count;	//!< The number of pages for the primary section.
	uint32_t secondary_start_page;	//!< The secondary section start flash page.
	uint32_t secondary_page_count;	//!< The number of pages for the secondary section.
} mal_e3prom_init_s;

typedef struct {
	mal_e3prom_init_s e3prom_init;
} mal_async_e3prom_init_s;

/**
 * These are the possible states for the primary and secondary section.
 */
typedef enum {
	MAL_E3PROM_STATE_ERASED = 1,         //!< When a section is in this state, it should be erased and only contain this state.
	MAL_E3PROM_STATE_ACTIVE = 2,         //!< When a section is actively used to read and write.
	MAL_E3PROM_STATE_DECOMMISSIONED = 3, //!< When a switch of active section occurs, the previous section is set to this state.
	MAL_E3PROM_STATE_INITIALIZING = 4,   //!< When a switch of active section occurs, the new active section set first to this state.
	MAL_E3PROM_STATE_UNKNOWN = 0xFFFFFFFF//!< MAL_E3PROM_STATE_UNKNOWN
} mal_e3prom_state_e;

/**
 * Possible sections.
 */
typedef enum {
	MAL_E3PROM_SECTION_PRIMARY = 0,  //!< MAL_E3PROM_SECTION_PRIMARY
	MAL_E3PROM_SECTION_SECONDARY = 1,//!< MAL_E3PROM_SECTION_SECONDARY
	MAL_E3PROM_SECTION_SIZE = 2,     //!< MAL_E3PROM_SECTION_SIZE
} mal_e3prom_section_e;

/**
 * Variables of a section.
 */
typedef struct {
	uint32_t start_page;	//!< The start page of the section.
	uint32_t page_count;	//!< The page count of the section.
	uint64_t last_address;	//!< The last writable key/value address
} mal_e3prom_section_s;

/**
 * The variables of the e3prom.
 */
typedef struct {
	mal_e3prom_section_s sections[MAL_E3PROM_SECTION_SIZE];	//!< The sections of e3prom.
	mal_e3prom_section_e active_section;					//!< The currently active section.
} mal_e3prom_s;

typedef struct {
	mal_e3prom_s e3prom;
} mal_async_e3prom_s;

/**
 * Function used to filter values during a transfer of page. This allows to delete values in the e3prom.
 * @param handle The handle given with the function pointer.
 * @param e3prom The e3prom doing the page transfer.
 * @param key The key (address) to being transferred.
 * @param value The value being transferred.
 * @return Returns true to keep the value, false to discard it.
 */
typedef bool (*mal_e3prom_filter_t)(void *handle, mal_e3prom_s *e3prom, uint32_t key, uint32_t value);

/**
 * This function initializes the e3prom.
 * @param init The initialization parameters.
 * @param e3prom The e3prom variable to use for other operations.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_e3prom_init(mal_e3prom_init_s *init, mal_e3prom_s *e3prom);

/**
 * This functions retrieves a value from the e3prom.
 * @param e3prom The e3prom to read from.
 * @param key The key (address) to read from.
 * @param value A pointer to where to write value read.
 * @return If the key is found, #MAL_ERROR_OK is returned. Returns
 * #MAL_ERROR_NOT_FOUND otherwise.
 */
mal_error_e mal_e3prom_get_value(mal_e3prom_s *e3prom, uint32_t key, uint32_t *value);

/**
 * This functions writes to the e3prom.
 * @param e3prom The e3prom to write to.
 * @param key The key (address) to write to.
 * @param value The value to write.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_e3prom_write_value(mal_e3prom_s *e3prom, uint32_t key, uint32_t value);

/**
 * This function triggers a page transfer to allow filtering of the e3prom. Using the filter, you basically delete
 * elements from the e3prom.
 * @param e3prom The e3prom to filter.
 * @param filter The filtering function.
 * @param handle The handle to pass to the filter.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_e3prom_filter(mal_e3prom_s *e3prom, mal_e3prom_filter_t filter, void *handle);

mal_error_e mal_async_e3prom_init(mal_async_e3prom_init_s *init, mal_async_e3prom_s *async_e3prom);

mal_error_e mal_async_e3prom_get_value(mal_async_e3prom_s *async_e3prom, uint32_t key, uint32_t *value);

mal_error_e mal_async_e3prom_write_value(mal_async_e3prom_s *async_e3prom, uint32_t key, uint32_t value);

#endif /* UTILS_MAL_E3PROM_H_ */
