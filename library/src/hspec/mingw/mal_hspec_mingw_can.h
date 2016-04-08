/*
 * mal_hspec_mingw_can.h
 *
 *  Created on: Mar 24, 2016
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

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_CAN_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_CAN_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_mingw_get_valid_can_ios(mal_hspec_can_e interface, const mal_hspec_gpio_s **txs, uint8_t *txs_size, const mal_hspec_gpio_s **rxs, uint8_t *rxs_size);

mal_error_e mal_hspec_mingw_can_init(mal_hspec_can_init_s *init);

mal_error_e mal_hspec_mingw_can_transmit(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg);

mal_error_e mal_hspec_mingw_can_get_tx_msg(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg);

mal_error_e mal_hspec_mingw_can_add_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter);

mal_error_e mal_hspec_mingw_can_remove_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_CAN_H_ */
