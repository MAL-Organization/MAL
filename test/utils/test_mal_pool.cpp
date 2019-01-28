/*
 * Copyright (c) 2019 Olivier Allaire
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

class TestMalPool : public ::testing::Test {
protected:
	static const int pool_objects_buffer_size = 10 * sizeof(mal_pool_object_s);
	uint8_t *pool_objects_buffer = NULL;
	static const int objects_buffer_size = 10 * sizeof(uint32_t);
	uint8_t *objects_buffer = NULL;
	mal_pool_s test_pool;

	void SetUp();
	void TearDown();

};

void TestMalPool::SetUp() {
	// Reserve pool objects
	this->pool_objects_buffer = (uint8_t*)malloc((size_t)this->pool_objects_buffer_size);
	// Initialize pool objects
	for (int i = 0; i < this->pool_objects_buffer_size; i++) {
		this->pool_objects_buffer[i] = 0;
	}
	// Reserve objects
	this->objects_buffer = (uint8_t*)malloc((size_t)this->objects_buffer_size);
	// Initialize objects
	for (int i = 0; i < this->objects_buffer_size; i++) {
		this->objects_buffer[i] = 0;
	}
	// Initialize pool
	mal_pool_init((mal_pool_object_s*)this->pool_objects_buffer,
				  this->pool_objects_buffer_size / sizeof(mal_pool_object_s),
				  this->objects_buffer, sizeof(uint32_t), &this->test_pool);
}

void TestMalPool::TearDown() {
	free(this->pool_objects_buffer);
	free(this->objects_buffer);
}

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

TEST_F (TestMalPool, GetNext) {
	mal_error_e result;
	uint64_t index;
	uint32_t *test_object;
	// Fill pool
	for (index = 0; index < this->test_pool.size; index++) {
		result = mal_pool_allocate(&this->test_pool, (void**)&test_object);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to allocate";
		// Set value equal to index
		*test_object = (uint32_t)index;
	}
	// Free every other object
	uint32_t free_count = 0;
	for (index = 0; index < this->test_pool.size; index += 2) {
		test_object = (uint32_t*)this->test_pool.objects[index].object;
		mal_pool_free(&this->test_pool, test_object);
		free_count++;
	}
	// Test get next function
	uint32_t expected_test_count = (uint32_t)(this->test_pool.size - free_count);
	uint32_t expected_value = 1;
	uint32_t test_count = 0;
	index = 0;
	do {
		// Make sure this loop hasn't been executed too many times
		ASSERT_LE(test_count, expected_test_count) << "To many objects return by get next";
		// Do get
		result = mal_pool_get_next(&this->test_pool, &index, (void**)&test_object);
		// Check if value if to be expected
		if (MAL_ERROR_OK == result) {
			test_count++;
			ASSERT_EQ(*test_object, expected_value) << "Did not get expected object";
			expected_value += 2;
		}
	} while (MAL_ERROR_OK == result);
	// Ensure the method got the correct number of objects
	ASSERT_EQ(test_count, expected_test_count) << "Return too few objects";
}

TEST_F (TestMalPool, ForEach) {
	mal_error_e result = MAL_ERROR_OK;
	uint64_t index;
	uint32_t *test_object;
	// Fill pool
	for (index = 0; index < this->test_pool.size; index++) {
		result = mal_pool_allocate(&this->test_pool, (void**)&test_object);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to allocate";
		// Set value equal to index
		*test_object = (uint32_t)index;
	}
	// Free every other object
	uint32_t free_count = 0;
	for (index = 0; index < this->test_pool.size; index += 2) {
		test_object = (uint32_t*)this->test_pool.objects[index].object;
		mal_pool_free(&this->test_pool, test_object);
		free_count++;
	}
	// Test for each
	uint32_t expected_test_count = (uint32_t)(this->test_pool.size - free_count);
	uint32_t expected_value = 1;
	uint32_t test_count = 0;
	MAL_POOL_FOR_EACH(&this->test_pool, index, test_object) {
		// Make sure this loop hasn't been executed too many times
		ASSERT_LE(test_count, expected_test_count) << "To many objects return by get next";
		// Check if value if to be expected
		if (MAL_ERROR_OK == result) {
			test_count++;
			ASSERT_EQ(*test_object, expected_value) << "Did not get expected object";
			expected_value += 2;
		}
	}
	// Ensure the method got the correct number of objects
	ASSERT_EQ(test_count, expected_test_count) << "Return too few objects";
}
