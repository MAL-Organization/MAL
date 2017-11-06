/*
 * mal_hspec_mingw_serial.h
 *
 *  Created on: Nov 6, 2017
 *      Author: Olivier
 */
/*
 * Copyright (c) 2017 Olivier Allaire
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

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_SERIAL_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_SERIAL_H_

#include "serial/mal_serial.h"

mal_error_e mal_hspec_mingw_serial_get_tx_data(mal_serial_port_e port, uint16_t *data);

mal_error_e mal_hspec_mingw_serial_push_rx_data(mal_serial_port_e port, uint16_t data);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_SERIAL_H_ */
