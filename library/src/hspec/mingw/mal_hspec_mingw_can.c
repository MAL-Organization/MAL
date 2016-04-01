/*
 * mal_hspec_mingw_can.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */

#include "mal_hspec_mingw_can.h"
#include "mal_hspec_mingw_cmn.h"
#include "utils/mal_circular_buffer.h"

#define MESSAGE_BUFFER_SIZE	100

typedef struct {
	mal_hspec_can_init_s init;
	mal_circular_buffer_s tx_circular_buffer;
	mal_hspec_can_msg_s message_buffer[MESSAGE_BUFFER_SIZE];
} mingw_can_interface_s;

static mingw_can_interface_s can_interfaces[MAL_HSPEC_CAN_SIZE];

mal_error_e mal_hspec_mingw_get_valid_can_ios(mal_hspec_can_e interface, const mal_hspec_gpio_s **txs, uint8_t *txs_size, const mal_hspec_gpio_s **rxs, uint8_t *rxs_size) {
	// Fetch IOs
	mal_hspec_mingw_cmn_valid_ios(txs, txs_size);
	*rxs = *txs;
	*rxs_size = *txs_size;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_can_init(mal_hspec_can_init_s *init) {
	// Save init
	can_interfaces[init->interface].init = *init;
	// Initialise circular buffer
	mal_circular_buffer_init((void*)can_interfaces[init->interface].message_buffer,
							 sizeof(mal_hspec_can_msg_s),
							 sizeof(mal_hspec_can_msg_s) * MESSAGE_BUFFER_SIZE,
							 &can_interfaces[init->interface].tx_circular_buffer);
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_can_transmit(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg) {
	// Write to buffer
	return mal_circular_buffer_write(&can_interfaces[interface].tx_circular_buffer, msg);
}

mal_error_e mal_hspec_mingw_can_get_tx_msg(mal_hspec_can_e interface, mal_hspec_can_msg_s *msg) {
	return mal_circular_buffer_read(&can_interfaces[interface].tx_circular_buffer, msg);
}

mal_error_e mal_hspec_mingw_can_add_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter) {
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_can_remove_filter(mal_hspec_can_e interface, mal_hspec_can_filter_s *filter) {
	return MAL_ERROR_OK;
}
