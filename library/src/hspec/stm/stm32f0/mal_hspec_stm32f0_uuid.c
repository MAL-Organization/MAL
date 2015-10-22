/*
 * mal_hspec_stm32f0_uuid.c
 *
 *  Created on: Jul 16, 2015
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

#include "mal_hspec_stm32f0_uuid.h"
#include "stm32f0/stm32f0xx_flash.h"

mal_error_e mal_hspec_stm32f0_uuid_read(uint64_t *uuid) {
	FLASH_Unlock();
	*uuid = *((volatile uint32_t*)0x1FFFF7AC);
	FLASH_Lock();
	return MAL_ERROR_OK;
}