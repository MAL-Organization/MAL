/*
 * mal_hspec_mingw_can.h
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
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
