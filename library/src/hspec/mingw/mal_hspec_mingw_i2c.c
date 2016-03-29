/*
 * mal_hspec_mingw_i2c.c
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */

#include "mal_hspec_mingw_i2c.h"
#include "mal_hspec_mingw_cmn.h"
#include "utils/mal_circular_buffer.h"

#define MESSAGE_BUFFER_SIZE	100

typedef struct {
	mal_hspec_i2c_init_s init;
	mal_circular_buffer_s tx_circular_buffer;
	mal_hspec_i2c_msg_s message_buffer[MESSAGE_BUFFER_SIZE];
} mingw_i2c_interface_s;

static mingw_i2c_interface_s i2c_interfaces[MAL_HSPEC_I2C_SIZE];

mal_error_e mal_hspec_mingw_get_valid_i2c_ios(mal_hspec_i2c_e interface, const mal_hspec_gpio_s **scls, uint8_t *scls_size, const mal_hspec_gpio_s **sdas, uint8_t *sdas_size) {
	// Fetch IOs
	mal_hspec_mingw_cmn_valid_ios(scls, scls_size);
	*sdas = *scls;
	*sdas_size = *scls_size;

	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_i2c_master_init(mal_hspec_i2c_init_s *init) {
	// Save init
	i2c_interfaces[init->interface].init = *init;
	// Initialise circular buffer
	return mal_circular_buffer_init((void*)i2c_interfaces[init->interface].message_buffer,
							 	 	sizeof(mal_hspec_i2c_msg_s),
									sizeof(mal_hspec_i2c_msg_s) * MESSAGE_BUFFER_SIZE,
									&i2c_interfaces[init->interface].tx_circular_buffer);
}

mal_error_e mal_hspec_mingw_i2c_transfer(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg) {
	// Write to buffer
	return mal_circular_buffer_write(&i2c_interfaces[interface].tx_circular_buffer, msg);
}

mal_error_e mal_hspec_mingw_i2c_get_transfer_msg(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg) {
	return mal_circular_buffer_read(&i2c_interfaces[interface].tx_circular_buffer, msg);
}
