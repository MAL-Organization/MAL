/*
 * test_mal_circular_buffer.cpp
 *
 *  Created on: Mar 22, 2016
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

extern "C" {
#include "utils/mal_circular_buffer.h"
}


#include "gtest/gtest.h"
#include <stdint.h>

class TestMalCircularBuffer : public ::testing::Test {
protected:
	static const int buffer_size = 10 * sizeof(uint32_t);
	uint8_t *buffer = NULL;
	mal_circular_buffer_s test_circular_buffer;

	virtual void SetUp() {
		// Reserve buffer
		this->buffer = (uint8_t*)malloc(this->buffer_size);
		// Initialize buffer
		for (int i = 0; i < this->buffer_size; i++) {
			this->buffer[i] = 0;
		}
		// Initialize circular buffer
		mal_circular_buffer_init(this->buffer,
								 sizeof(uint32_t),
								 this->buffer_size,
								 &this->test_circular_buffer);
	}

	virtual void TearDown() {
		free(this->buffer);
	}

};

TEST_F(TestMalCircularBuffer, Write) {
	mal_error_e result;
	uint32_t test_data = 0x42;
	// Allocate object
	result = mal_circular_buffer_write(&this->test_circular_buffer,
									   &test_data);
	// Check result
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to buffer";
	// Check buffer
	ASSERT_EQ(*((uint32_t*)this->buffer), 0x42) << "First 4 bytes should contain a uint32 of value 0x42.";
	// Check the rest of the buffer
	for (uint32_t i = 1; i < this->buffer_size / sizeof(uint32_t); i++) {
		ASSERT_EQ(*(((uint32_t*)this->buffer) + i), 0);
	}
}
