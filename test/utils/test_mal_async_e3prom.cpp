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
#include "utils/mal_e3prom.h"
#include "hspec/gnu/mal_hspec_gnu_flash.h"
#include "flash/mal_flash.h"
}

#include "gtest/gtest.h"

#define TEST_MAL_ASYNC_E3PROM_PENDING_WRITE_ARRAY_SIZE  10
#define TEST_MAL_ASYNC_E3PROM_PENDING_FILTER_ARRAY_SIZE 2

class TestMalAsyncE3prom : public ::testing::Test {
protected:
    static const int first_page_size = 128;
    static const int second_page_size = 256;
    static const uint64_t burst_size = 10;
    mal_hspec_gnu_flash_info_s flash_info;

    void SetUp();
    void TearDown();

    static bool filter_key_2(void *handle, mal_e3prom_s *e3prom, uint32_t key, uint32_t value);
    static bool filter_key_3(void *handle, mal_e3prom_s *e3prom, uint32_t key, uint32_t value);
    static void filter_complete(void *handle, mal_async_e3prom_s *async_e3prom);

    static mal_e3prom_search_result_e search(void *handle, mal_async_e3prom_s *async_e3prom, uint32_t key,
                                             uint32_t value);

    mal_async_e3prom_pending_write_s pending_write_array[TEST_MAL_ASYNC_E3PROM_PENDING_WRITE_ARRAY_SIZE];
    mal_pool_object_s pending_write_object_array[TEST_MAL_ASYNC_E3PROM_PENDING_WRITE_ARRAY_SIZE];
    mal_async_e3prom_pending_filter_s pending_filter_array[TEST_MAL_ASYNC_E3PROM_PENDING_FILTER_ARRAY_SIZE];
    mal_pool_object_s pending_filter_object_array[TEST_MAL_ASYNC_E3PROM_PENDING_FILTER_ARRAY_SIZE];
    mal_async_e3prom_s async_e3prom;
    bool filter_complete_called;
    uint32_t search_count;
    uint32_t search_key;
    uint32_t search_value;
};

void TestMalAsyncE3prom::SetUp() {
    mal_error_e result;
    this->filter_complete_called = false;
    this->search_count = 0;
    // Set flash info
    this->flash_info.page_count = 4;
    this->flash_info.pages = (mal_hspec_gnu_flash_page_info_s*)malloc(sizeof(mal_hspec_gnu_flash_page_info_s) * this->flash_info.page_count);
    this->flash_info.pages[0].size = (uint32_t)this->first_page_size;
    this->flash_info.pages[1].size = (uint32_t)this->second_page_size;
    this->flash_info.pages[2].size = (uint32_t)this->first_page_size;
    this->flash_info.pages[3].size = (uint32_t)this->second_page_size;
    mal_hspec_gnu_flash_set_flash(&this->flash_info);
    // Initialize e3prom
    mal_async_e3prom_init_s init;
    init.e3prom_init.primary_start_page = 0;
    init.e3prom_init.primary_page_count = 2;
    init.e3prom_init.secondary_start_page = 2;
    init.e3prom_init.secondary_page_count = 2;
    init.burst_size = this->burst_size;
    init.pending_write_array = this->pending_write_array;
    init.pending_write_object_array = this->pending_write_object_array;
    init.pending_write_array_size = TEST_MAL_ASYNC_E3PROM_PENDING_WRITE_ARRAY_SIZE;
    init.pending_filter_array = this->pending_filter_array;
    init.pending_filter_object_array = this->pending_filter_object_array;
    init.pending_filter_array_size = TEST_MAL_ASYNC_E3PROM_PENDING_FILTER_ARRAY_SIZE;
    result = mal_async_e3prom_init(&init, &this->async_e3prom);
    ASSERT_EQ(result, MAL_ERROR_OK);
}

void TestMalAsyncE3prom::TearDown() {
    free(this->flash_info.pages);
}

bool TestMalAsyncE3prom::filter_key_2(void *handle, mal_e3prom_s *e3prom, uint32_t key, uint32_t value) {
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(e3prom);
    MAL_DEFS_UNUSED(value);
    return 2 != key;
}

bool TestMalAsyncE3prom::filter_key_3(void *handle, mal_e3prom_s *e3prom, uint32_t key, uint32_t value) {
    MAL_DEFS_UNUSED(handle);
    MAL_DEFS_UNUSED(e3prom);
    MAL_DEFS_UNUSED(value);
    return 3 != key;
}

void TestMalAsyncE3prom::filter_complete(void *handle, mal_async_e3prom_s *async_e3prom) {
    MAL_DEFS_UNUSED(async_e3prom);
    TestMalAsyncE3prom *test = (TestMalAsyncE3prom*)handle;
    test->filter_complete_called = true;
}

mal_e3prom_search_result_e TestMalAsyncE3prom::search(void *handle, mal_async_e3prom_s *async_e3prom, uint32_t key,
                                                      uint32_t value) {
    MAL_DEFS_UNUSED(async_e3prom);
    TestMalAsyncE3prom *test = (TestMalAsyncE3prom*)handle;
    test->search_count++;
    if (test->search_key == key) {
        test->search_value = value;
        return MAL_E3PROM_SEARCH_RESULT_FOUND;
    }
    return MAL_E3PROM_SEARCH_RESULT_CONTINUE;
}

TEST_F(TestMalAsyncE3prom, WriteAndRead) {
    mal_error_e result;
    // We will write 4 different times
    for (uint32_t i = 0; i < 4; i++) {
        uint32_t test_value = 0x42 * i;
        // We will write 4 different keys
        result = mal_async_e3prom_write_value(&this->async_e3prom, 0, test_value);
        ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
        result = mal_async_e3prom_write_value(&this->async_e3prom, 1, test_value + 1);
        ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
        result = mal_async_e3prom_write_value(&this->async_e3prom, 2, test_value + 2);
        ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
        result = mal_async_e3prom_write_value(&this->async_e3prom, 3, test_value + 3);
        ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
        // We will read back values
        uint32_t read_value;
        result = mal_async_e3prom_get_value(&this->async_e3prom, 0, &read_value);
        ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
        ASSERT_EQ(read_value, test_value) << "Read value mismatch";
        result = mal_async_e3prom_get_value(&this->async_e3prom, 1, &read_value);
        ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
        ASSERT_EQ(read_value, test_value + 1) << "Read value mismatch";
        result = mal_async_e3prom_get_value(&this->async_e3prom, 2, &read_value);
        ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
        ASSERT_EQ(read_value, test_value + 2) << "Read value mismatch";
        result = mal_async_e3prom_get_value(&this->async_e3prom, 3, &read_value);
        ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
        ASSERT_EQ(read_value, test_value + 3) << "Read value mismatch";
    }
}

/**
 * Since long operations are async, make sure writes are not dropped when a switch starts.
 */
TEST_F(TestMalAsyncE3prom, WriteSectionSwitch) {
    mal_error_e result;
    // Make sure primary section is active
    ASSERT_EQ(this->async_e3prom.e3prom.active_section,
              MAL_E3PROM_SECTION_PRIMARY) << "Primary section should be active";
    // Write enough times to switch sections
    uint32_t rom_size = (this->first_page_size + this->second_page_size) / (sizeof(uint32_t) * 2);
    for (uint32_t i = 0; i < rom_size; i++) {
        uint32_t read_value;
        result = mal_async_e3prom_write_value(&this->async_e3prom, 0, i);
        ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
        result = mal_async_e3prom_get_value(&this->async_e3prom, 0, &read_value);
        ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
        ASSERT_EQ(read_value, i) << "Read value mismatch";
    }
    // Make sure we switched to secondary section
    ASSERT_EQ(this->async_e3prom.e3prom.active_section,
              MAL_E3PROM_SECTION_SECONDARY) << "Secondary section should be active";
    ASSERT_TRUE(this->async_e3prom.switch_in_progress);
    // Write an extra value
    uint32_t test_key = 2;
    uint32_t test_value = 42;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key, test_value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    // Allow switch to end
    for (int i = 0; i < 100; i++) {
        result = mal_async_e3prom_process(&this->async_e3prom);
        ASSERT_EQ(result, MAL_ERROR_OK);
    }
    ASSERT_FALSE(this->async_e3prom.switch_in_progress);
    // Read back test value
    uint32_t read_value;
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key, &read_value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(read_value, test_value);
}

/**
 * Test filter which ends at filter call.
 */
TEST_F(TestMalAsyncE3prom, Filter) {
    mal_error_e result;
    // Write 3 values
    uint32_t test_key_1 = 1;
    uint32_t test_value_1 = 41;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key_1, test_value_1);
    ASSERT_EQ(result, MAL_ERROR_OK);
    uint32_t test_key_2 = 2;
    uint32_t test_value_2 = 42;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key_2, test_value_2);
    ASSERT_EQ(result, MAL_ERROR_OK);
    uint32_t test_key_3 = 3;
    uint32_t test_value_3 = 43;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key_3, test_value_3);
    ASSERT_EQ(result, MAL_ERROR_OK);
    // Make sure all 3 values can be read
    uint32_t value;
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_1, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_1);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_2, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_2);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_3, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_3);
    // Filter second value. Force this to end in single burst size
    this->async_e3prom.burst_size = UINT64_MAX;
    result = mal_async_e3prom_filter(&this->async_e3prom, this->filter_key_2, this->filter_complete, this);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_TRUE(this->filter_complete_called);
    // Make sure value 1 and 3 can still be read
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_1, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_1);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_3, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_3);
    // Make sure 2 was removed
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_2, &value);
    ASSERT_EQ(result, MAL_ERROR_NOT_FOUND);
}

/**
 * Test filter which ends in async way.
 */
TEST_F(TestMalAsyncE3prom, FilterAsync) {
    mal_error_e result;
    // Write 3 values
    uint32_t test_key_1 = 1;
    uint32_t test_value_1 = 41;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key_1, test_value_1);
    ASSERT_EQ(result, MAL_ERROR_OK);
    uint32_t test_key_2 = 2;
    uint32_t test_value_2 = 42;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key_2, test_value_2);
    ASSERT_EQ(result, MAL_ERROR_OK);
    uint32_t test_key_3 = 3;
    uint32_t test_value_3 = 43;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key_3, test_value_3);
    ASSERT_EQ(result, MAL_ERROR_OK);
    // Make sure all 3 values can be read
    uint32_t value;
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_1, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_1);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_2, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_2);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_3, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_3);
    // Filter second value. Force this to end in single burst size
    result = mal_async_e3prom_filter(&this->async_e3prom, this->filter_key_2, this->filter_complete, this);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_FALSE(this->filter_complete_called);
    // All 3 values should still be there also
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_1, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_1);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_2, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_2);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_3, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_3);
    // Writes should be buffered until filter is complete
    uint32_t test_key_4 = 4;
    uint32_t test_value_4 = 44;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key_4, test_value_4);
    ASSERT_EQ(result, MAL_ERROR_OK);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_4, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_4);
    // Allow filter to end
    for (int i = 0; i < 100; i++) {
        result = mal_async_e3prom_process(&this->async_e3prom);
        ASSERT_EQ(result, MAL_ERROR_OK);
    }
    ASSERT_FALSE(this->async_e3prom.switch_in_progress);
    ASSERT_TRUE(this->filter_complete_called);
    // Make sure value 1 and 3 can still be read
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_1, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_1);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_3, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_3);
    // Make sure 2 was removed
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_2, &value);
    ASSERT_EQ(result, MAL_ERROR_NOT_FOUND);
}

/**
 * Test filter which ends in async way and a second filter is waiting.
 */
TEST_F(TestMalAsyncE3prom, FilterPending) {
    mal_error_e result;
    // Write 3 values
    uint32_t test_key_1 = 1;
    uint32_t test_value_1 = 41;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key_1, test_value_1);
    ASSERT_EQ(result, MAL_ERROR_OK);
    uint32_t test_key_2 = 2;
    uint32_t test_value_2 = 42;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key_2, test_value_2);
    ASSERT_EQ(result, MAL_ERROR_OK);
    uint32_t test_key_3 = 3;
    uint32_t test_value_3 = 43;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key_3, test_value_3);
    ASSERT_EQ(result, MAL_ERROR_OK);
    // Make sure all 3 values can be read
    uint32_t value;
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_1, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_1);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_2, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_2);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_3, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_3);
    // Filter second value. Force this to end in single burst size
    result = mal_async_e3prom_filter(&this->async_e3prom, this->filter_key_2, this->filter_complete, this);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_FALSE(this->filter_complete_called);
    // All 3 values should still be there also
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_1, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_1);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_2, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_2);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_3, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_3);
    // Request a second filter
    result = mal_async_e3prom_filter(&this->async_e3prom, this->filter_key_3, this->filter_complete, this);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_FALSE(this->filter_complete_called);
    // Allow first filter to end
    for (int i = 0; i < 100; i++) {
        result = mal_async_e3prom_process(&this->async_e3prom);
        ASSERT_EQ(result, MAL_ERROR_OK);
        if (this->filter_complete_called) {
            break;
        }
    }
    ASSERT_TRUE(this->async_e3prom.switch_in_progress);
    ASSERT_TRUE(this->filter_complete_called);
    this->filter_complete_called = false;
    // Make sure value 1 and 3 can still be read
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_1, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_1);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_3, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_3);
    // Make sure 2 was removed
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_2, &value);
    ASSERT_EQ(result, MAL_ERROR_NOT_FOUND);
    // Allow second filter to end
    for (int i = 0; i < 100; i++) {
        result = mal_async_e3prom_process(&this->async_e3prom);
        ASSERT_EQ(result, MAL_ERROR_OK);
    }
    ASSERT_FALSE(this->async_e3prom.switch_in_progress);
    ASSERT_TRUE(this->filter_complete_called);
    // Make sure value 1 can still be read
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_1, &value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(value, test_value_1);
    // Make sure 2 and 3 were removed
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_2, &value);
    ASSERT_EQ(result, MAL_ERROR_NOT_FOUND);
    result = mal_async_e3prom_get_value(&this->async_e3prom, test_key_3, &value);
    ASSERT_EQ(result, MAL_ERROR_NOT_FOUND);
}

TEST_F(TestMalAsyncE3prom, Search) {
    mal_error_e result;
    // Search for a not existing value
    this->search_count = 0;
    this->search_key = 0;
    result = mal_async_e3prom_search(&this->async_e3prom, this->search, this);
    ASSERT_EQ(result, MAL_ERROR_NOT_FOUND);
    uint32_t min_search_count = this->search_count;
    // Write value to have an existing one
    uint32_t test_key = 0x42;
    uint32_t test_value = 1;
    result = mal_async_e3prom_write_value(&this->async_e3prom, test_key, test_value);
    ASSERT_EQ(result, MAL_ERROR_OK);
    // Make sure search goes through all values
    this->search_count = 0;
    result = mal_async_e3prom_search(&this->async_e3prom, this->search, this);
    ASSERT_EQ(result, MAL_ERROR_NOT_FOUND);
    ASSERT_EQ(this->search_count, min_search_count + 1);
    // Search for stored value
    this->search_count = 0;
    this->search_key = test_key;
    this->search_value = test_value + 1;
    result = mal_async_e3prom_search(&this->async_e3prom, this->search, this);
    ASSERT_EQ(result, MAL_ERROR_OK);
    ASSERT_EQ(this->search_value, test_value);
}
