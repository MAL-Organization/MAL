/*
 * mal_uuid.h
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

#ifndef UUID_MAL_UUID_H_
#define UUID_MAL_UUID_H_

#include "std/mal_error.h"
#include "std/mal_stdint.h"

/**
 * @brief Fetch the uuid of the device. This should be unique ID for the device
 * of 64 bits.
 * @param uuid This is a pointer to uint64_t variable. The uuid will be stored
 * to this location.
 * @return Returns MAL_ERROR_OK on success.
 */
mal_error_e mal_uuid_read(uint64_t *uuid);

#endif /* UUID_MAL_UUID_H_ */
