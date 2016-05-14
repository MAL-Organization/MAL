/*
 * test_mal_pool.cpp
 *
 *  Created on: May 13, 2016
 *      Author: olivi
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
#include "utils/mal_pool.h"
}


#include "gtest/gtest.h"
#include <stdint.h>

class TestMalPool : public ::testing::Test {
protected:
	static const int pool_objects_buffer_size = 10 * sizeof(mal_pool_object_s);
	uint8_t *pool_objects_buffer = NULL;
	static const int objects_buffer_size = 10 * sizeof(uint32_t);
	uint8_t *objects_buffer = NULL;
	mal_pool_s test_pool;

	virtual void SetUp() {
		// Reserve pool objects
		this->pool_objects_buffer = (uint8_t*)malloc(this->pool_objects_buffer_size);
		// Initialize pool objects
		for (int i = 0; i < this->pool_objects_buffer_size; i++) {
			this->pool_objects_buffer[i] = 0;
		}
		// Reserve objects
		this->objects_buffer = (uint8_t*)malloc(this->objects_buffer_size);
		// Initialize objects
		for (int i = 0; i < this->objects_buffer_size; i++) {
			this->objects_buffer[i] = 0;
		}
		// Initialize pool
		mal_pool_init((mal_pool_object_s*)this->pool_objects_buffer,
					  this->pool_objects_buffer_size / sizeof(mal_pool_object_s),
					  this->objects_buffer, sizeof(uint32_t), &this->test_pool);
	}

	virtual void TearDown() {
		free(this->pool_objects_buffer);
		free(this->objects_buffer);
	}

};

TEST_F(TestMalPool, Allocate) {
	mal_error_e result;
	uint32_t *test_object;
	// Allocate
	result = mal_pool_allocate(&this->test_pool, (void**)&test_object);
	// Check result
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to allocate";
	// Check state
	ASSERT_FALSE(this->test_pool.objects[0].is_free) << "First object should not be free.";
	// Set value
	*test_object = 0x42;
	// Check pool
	ASSERT_EQ(*((uint32_t*)this->test_pool.objects[0].object), 0x42) << "First object should contain a uint32 of value 0x42.";
	// Check the rest of the buffer
	for (uint32_t i = 1; i < this->objects_buffer_size / sizeof(uint32_t); i++) {
		ASSERT_EQ(*(((uint32_t*)this->objects_buffer) + i), 0);
	}
}

TEST_F (TestMalPool, Free) {
	mal_error_e result;
	uint32_t *test_object;
	// Allocate
	result = mal_pool_allocate(&this->test_pool, (void**)&test_object);
	// Check result
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to allocate";
	// Check state
	ASSERT_FALSE(this->test_pool.objects[0].is_free) << "First object should not be free.";
	// Free object
	mal_pool_free(&this->test_pool, test_object);
	// Check state
	ASSERT_TRUE(this->test_pool.objects[0].is_free) << "First object should be free.";
}

TEST_F (TestMalPool, Flush) {
	uint32_t *test_object;
	// Allocate all objects
	while(MAL_ERROR_OK == mal_pool_allocate(&this->test_pool, (void**)&test_object));
	// Check that all objects are allocated
	for (uint64_t i = 0; i < this->test_pool.size; i++) {
		ASSERT_FALSE(this->test_pool.objects[i].is_free) << "All objects should be allocated";
	}
	// Flush pool
	mal_pool_flush(&this->test_pool);
	// All objects should be free
	for (uint64_t i = 0; i < this->test_pool.size; i++) {
		ASSERT_TRUE(this->test_pool.objects[i].is_free) << "All objects should be free";
	}
}
