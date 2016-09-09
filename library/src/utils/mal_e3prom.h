/**
 * @file mal_e3prom.h
 * @author Olivier Allaire
 * @date September 6 2016
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
 * @brief Emulated electrically erasable programmable read-only memory utility.
 * @note Emulated eeprom is the most ridiculous and incongruous acronym ever.
 * It's just more than 30 years of just adding adjectives to adapt to
 * technology, seriously!
 */

#ifndef UTILS_MAL_E3PROM_H_
#define UTILS_MAL_E3PROM_H_

#include "std/mal_stdint.h"
#include "std/mal_error.h"

#define MAL_E3PROM_STATE_KEY	0xFFFFFFFE

typedef struct {
	uint32_t primary_start_page;
	uint32_t primary_page_count;
	uint32_t secondary_start_page;
	uint32_t secondary_page_count;
} mal_e3prom_init_s;

typedef enum {
	MAL_E3PROM_STATE_ERASED = 1,
	MAL_E3PROM_STATE_ACTIVE = 2,
	MAL_E3PROM_STATE_DECOMMISSIONED = 3,
	MAL_E3PROM_STATE_INITIALIZING = 4,
	MAL_E3PROM_STATE_UNKNOWN = 0xFFFFFFFF
} mal_e3prom_state_e;

typedef enum {
	MAL_E3PROM_SECTION_PRIMARY = 0,
	MAL_E3PROM_SECTION_SECONDARY = 1,
	MAL_E3PROM_SECTION_SIZE = 2,
} mal_e3prom_section_e;

typedef struct {
	uint32_t start_page;
	uint32_t page_count;
	uint64_t last_address;
} mal_e3prom_section_s;

typedef struct {
	mal_e3prom_section_s sections[MAL_E3PROM_SECTION_SIZE];
	mal_e3prom_section_e active_section;
} mal_e3prom_s;

mal_error_e mal_e3prom_init(mal_e3prom_init_s *init, mal_e3prom_s *e3prom);

mal_error_e mal_e3prom_get_value(mal_e3prom_s *e3prom, uint32_t key, uint32_t *value);

mal_error_e mal_e3prom_write_value(mal_e3prom_s *e3prom, uint32_t key, uint32_t value);

#endif /* UTILS_MAL_E3PROM_H_ */
