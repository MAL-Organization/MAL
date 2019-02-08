/**
 * @file mal_e3prom.h
 * @author Olivier Allaire
 * @date February 8 2019
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
#include "mal_pool.h"

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

/**
 * Structure to store pending writes. Used for internal functions of asynchronous e3prom.
 */
typedef struct {
	uint32_t key;   //!< The key of the pending write.
	uint32_t value; //!< The value of the pending write.
} mal_async_e3prom_pending_write_s;

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
	mal_e3prom_section_e active_section;					    //!< The currently active section.
} mal_e3prom_s;

/**
 * Function used to filter values during a transfer of page. This allows to delete values in the e3prom.
 * @param handle The handle given with the function pointer.
 * @param e3prom The e3prom doing the page transfer.
 * @param key The key (address) to being transferred.
 * @param value The value being transferred.
 * @return Returns true to keep the value, false to discard it.
 */
typedef bool (*mal_e3prom_filter_t)(void *handle, mal_e3prom_s *e3prom, uint32_t key, uint32_t value);

/// @cond SKIP
typedef struct MAL_ASYNC_E3PROM mal_async_e3prom_s;
/// @endcond

/**
 * Function to know when a filter operation is complete when using an async e3prom.
 * @param handle The handle given with the function pointer.
 * @param async_e3prom The e3prom doing the page transfer.
 */
typedef void (*mal_async_e3prom_filter_complete_t)(void *handle, mal_async_e3prom_s *async_e3prom);

/**
 * The variables of the asynchronous e3prom.
 */
typedef struct MAL_ASYNC_E3PROM {
	mal_e3prom_s e3prom;                                        //!< The variables of the e3prom.
	// Sector switch variables
	uint64_t secsw_previous_key_address;                        //!< Used to split section switches.
	uint64_t burst_size;                                        //!< Maximum operations for one execution of a section switch.
	bool switch_in_progress;                                    //!< Flag to track section switches.
	mal_e3prom_filter_t active_filter;                          //!< The active filter during an asynchronous filter.
	mal_async_e3prom_filter_complete_t active_filter_complete;  //!< The active filter complete during an asynchronous filter.
	void *active_filter_handle;                                 //!< The active filter handle during an asynchronous filter.
	// Pending writes variables
	mal_pool_s pending_writes_pool;                             //!< Used to track pending writes during section switch.
	// Pending filters variables
	mal_pool_s pending_filters_pool;                            //!< Used to track pending filters during section switch.
} mal_async_e3prom_s;

/**
 * Structure to store pending filters. Used for internal functions of asynchronous e3prom.
 */
typedef struct {
	mal_e3prom_filter_t filter;                         //!< The pending filter function.
	mal_async_e3prom_filter_complete_t filter_complete; //!< The pending filter complete function.
	void *handle;                                       //!< The pending filter handle.
} mal_async_e3prom_pending_filter_s;

/**
 * These are the initialization parameters of the asynchronous e3prom.
 */
typedef struct {
	mal_e3prom_init_s e3prom_init;                              //!< These are the initialization parameters of the e3prom.
	uint64_t burst_size;                                        //!< This limits long running operations. Change this number make each process pass shorter.
	mal_async_e3prom_pending_write_s *pending_write_array;      //!< This should point to an array of mal_async_e3prom_pending_write_s. Will be used to store pending writes.
	mal_pool_object_s *pending_write_object_array;              //!< This should point to an array of mal_pool_object_s. Must be the same size as pending_write_array.
	uint64_t pending_write_array_size;                          //!< The length of the pending_write_array size.
	mal_async_e3prom_pending_filter_s *pending_filter_array;    //!< This should point to an array of mal_async_e3prom_pending_filter_s. Will be used to store pending filters.
	mal_pool_object_s *pending_filter_object_array;             //!< This should point to an array of mal_pool_object_s. Must be the same size as pending_filter_array.
	uint64_t pending_filter_array_size;                         //!< The length of the pending_filter_array size.
} mal_async_e3prom_init_s;

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

/**
 * This function initialises the asynchronous e3prom. The goal of the asynchronous e3prom is to prevent blocking
 * function calls to the e3prom. This occurs when a section switch is necessary. The draw back is to increase the
 * likelihood of loosing data during a page switch since it makes the operation longer. If the operation is longer, it
 * increases the chances of loosing power during the operation. Note that all the recovery mechanisms of the standard
 * e3prom applies to this.
 * @param init The initialization parameters.
 * @param async_e3prom The async_e3prom variable to use for other operations.
 * @return
 */
mal_error_e mal_async_e3prom_init(mal_async_e3prom_init_s *init, mal_async_e3prom_s *async_e3prom);

/**
 * This functions retrieves a value from the async_e3prom. The get will be instantaneous. The get disregards filters in
 * progress.
 * @param async_e3prom The async_e3prom to read from.
 * @param key The key (address) to read from.
 * @param value A pointer to where to write value read.
 * @return If the key is found, #MAL_ERROR_OK is returned. Returns
 * #MAL_ERROR_NOT_FOUND otherwise.
 */
mal_error_e mal_async_e3prom_get_value(mal_async_e3prom_s *async_e3prom, uint32_t key, uint32_t *value);

/**
 * This functions writes to the async_e3prom. If a section switch is started, it will be limited by the burst size
 * initialisation parameter. Call the mal_async_e3prom_process function periodically to allow section switch to end.
 * @param async_e3prom The async_e3prom to write to.
 * @param key The key (address) to write to.
 * @param value The value to write.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_async_e3prom_write_value(mal_async_e3prom_s *async_e3prom, uint32_t key, uint32_t value);

/**
 * Allows asynchronous operations to finish over multiple calls. This is controlled by the burst size initialisation
 * parameter.
 * @param async_e3prom The async_e3prom to process.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_async_e3prom_process(mal_async_e3prom_s *async_e3prom);

/**
 * This function triggers a page transfer to allow filtering of the asynchronous e3prom. Using the filter,
 * you basically delete elements from the e3prom. Since this operation is asynchronous, call mal_async_e3prom_process
 * function periodically to allow filter to finish. When finished, the mal_async_e3prom_filter_complete_t function will
 * be called.
 * @param async_e3prom The async_e3prom to filter.
 * @param filter The filtering function.
 * @param filter_complete Function to call when filter is done.
 * @param handle The handle to pass to the filter.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_async_e3prom_filter(mal_async_e3prom_s *async_e3prom, mal_e3prom_filter_t filter,
		                            mal_async_e3prom_filter_complete_t filter_complete, void *handle);

#endif /* UTILS_MAL_E3PROM_H_ */
