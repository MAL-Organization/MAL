/*
 * mal_hspec_mingw_i2c.c
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

#include <windows.h>
#undef interface
#include <process.h>

#include "mal_hspec_mingw_i2c.h"
#include "mal_hspec_mingw_cmn.h"
#include "utils/mal_circular_buffer.h"

#define MESSAGE_BUFFER_SIZE	100
#define I2C_LOCK_DELAY		1000

typedef struct {
	mal_hspec_i2c_init_s init;
	mal_circular_buffer_s tx_circular_buffer;
	mal_hspec_i2c_msg_s message_buffer[MESSAGE_BUFFER_SIZE];
	HANDLE mutex;
	bool interrupt_active;
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
	// Create mutex
	i2c_interfaces[init->interface].mutex = CreateMutex(NULL, FALSE, NULL);
	i2c_interfaces[init->interface].interrupt_active = true;
	// Initialise circular buffer
	mal_circular_buffer_init((void*)i2c_interfaces[init->interface].message_buffer,
							 sizeof(mal_hspec_i2c_msg_s),
							 sizeof(mal_hspec_i2c_msg_s) * MESSAGE_BUFFER_SIZE,
							 &i2c_interfaces[init->interface].tx_circular_buffer);
	return MAL_ERROR_OK;
}

mal_error_e mal_hspec_mingw_i2c_transfer(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg) {
	// Write to buffer
	return mal_circular_buffer_write(&i2c_interfaces[interface].tx_circular_buffer, msg);
}

mal_error_e mal_hspec_mingw_i2c_get_transfer_msg(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg) {
	return mal_circular_buffer_read(&i2c_interfaces[interface].tx_circular_buffer, msg);
}

bool mal_hspec_mingw_i2c_lock_interface(mal_hspec_i2c_e interface, uint32_t timeout_ms) {
	DWORD result = WaitForSingleObject(i2c_interfaces[interface].mutex, timeout_ms);
	return result == WAIT_OBJECT_0;
}

void mal_hspec_mingw_i2c_unlock_interface(mal_hspec_i2c_e interface) {
	ReleaseMutex(i2c_interfaces[interface].mutex);
}

bool mal_hspec_mingw_i2c_disable_interrupt(mal_hspec_i2c_e interface) {
	bool interface_state = i2c_interfaces[interface].interrupt_active;
	if (interface_state) {
		mal_hspec_mingw_i2c_lock_interface(interface, I2C_LOCK_DELAY);
		i2c_interfaces[interface].interrupt_active = false;
	}
	return interface_state;
}

void mal_hspec_mingw_i2c_enable_interrupt(mal_hspec_i2c_e interface, bool active) {
	if (active) {
		i2c_interfaces[interface].interrupt_active = true;
		mal_hspec_mingw_i2c_unlock_interface(interface);
	}
}
