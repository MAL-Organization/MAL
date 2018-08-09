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

#include "uuid/mal_uuid.h"
#include "stm32f7/stm32f7xx_hal_flash.h"

mal_error_e mal_uuid_read(uint64_t *uuid) {
    HAL_FLASH_Unlock();
    *uuid = *((volatile uint64_t*)0x1FF0F420);
    HAL_FLASH_Lock();
    return MAL_ERROR_OK;
}

