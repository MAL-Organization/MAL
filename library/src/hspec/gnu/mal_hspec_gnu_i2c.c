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

#include "mal_hspec_gnu_i2c.h"
#include "utils/mal_circular_buffer.h"
#include "pthread.h"
#include "std/mal_defs.h"

#define MESSAGE_BUFFER_SIZE	100
#define I2C_LOCK_DELAY		1000

typedef struct {
	mal_i2c_init_s init;
	mal_circular_buffer_s tx_circular_buffer;
	mal_i2c_msg_s message_buffer[MESSAGE_BUFFER_SIZE];
	pthread_mutex_t mutex;
	bool interrupt_active;
} gnu_i2c_interface_s;

static gnu_i2c_interface_s i2c_interfaces[MAL_I2C_SIZE];

mal_error_e mal_i2c_init_master(mal_i2c_init_s *init, mal_i2c_s *handle) {
	handle->interface = init->interface;
	// Save init
	i2c_interfaces[init->interface].init = *init;
	// Create mutex
	i2c_interfaces[init->interface].mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	i2c_interfaces[init->interface].interrupt_active = true;
	// Initialise circular buffer
	mal_circular_buffer_init((void*)i2c_interfaces[init->interface].message_buffer,
							 sizeof(mal_i2c_msg_s),
							 sizeof(mal_i2c_msg_s) * MESSAGE_BUFFER_SIZE,
							 &i2c_interfaces[init->interface].tx_circular_buffer);
	return MAL_ERROR_OK;
}

mal_error_e mal_i2c_master_direct_init(mal_i2c_init_s *init, const void *direct_init, mal_i2c_s *handle) {
	MAL_DEFS_UNUSED(direct_init);
    return mal_i2c_init_master(init, handle);
}

mal_error_e mal_i2c_transfer(mal_i2c_s *handle, mal_i2c_msg_s *msg) {
	// Write to buffer
	return mal_circular_buffer_write(&i2c_interfaces[handle->interface].tx_circular_buffer, msg);
}

mal_error_e mal_hspec_gnu_i2c_get_transfer_msg(mal_i2c_e interface, mal_i2c_msg_s *msg) {
	return mal_circular_buffer_read(&i2c_interfaces[interface].tx_circular_buffer, msg);
}

bool mal_hspec_gnu_i2c_lock_interface(mal_i2c_e interface, uint32_t timeout_ms) {
    int result;
    struct timespec timeout;
    uint32_t seconds = timeout_ms / 1000;
    timeout.tv_sec = seconds;
    timeout.tv_nsec = (timeout_ms - (seconds * 1000)) * 1000000;
    result = pthread_mutex_timedlock(&i2c_interfaces[interface].mutex, &timeout);
	return 0 == result;
}

void mal_hspec_gnu_i2c_unlock_interface(mal_i2c_e interface) {
    pthread_mutex_unlock(&i2c_interfaces[interface].mutex);
}

MAL_DEFS_INLINE void mal_i2c_disable_interrupt(mal_i2c_s *handle, mal_i2c_interrupt_state_s *state) {
	state->active = i2c_interfaces[handle->interface].interrupt_active;
	if (state->active) {
		mal_hspec_gnu_i2c_lock_interface(handle->interface, I2C_LOCK_DELAY);
		i2c_interfaces[handle->interface].interrupt_active = false;
	}
}

MAL_DEFS_INLINE void mal_i2c_restore_interrupt(mal_i2c_s *handle, mal_i2c_interrupt_state_s *state) {
	if (state->active) {
		i2c_interfaces[handle->interface].interrupt_active = true;
		mal_hspec_gnu_i2c_unlock_interface(handle->interface);
	}
}
