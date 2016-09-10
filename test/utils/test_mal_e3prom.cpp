/*
 * test_mal_e3prom.cpp
 *
 *  Created on: Sep 8, 2016
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
#include "utils/mal_e3prom.h"
#include "hspec/mingw/mal_hspec_mingw_flash.h"
}

#include "gtest/gtest.h"

class TestMalE3prom : public ::testing::Test {
protected:
	static const int first_page_size = 128;
	static const int second_page_size = 256;
	mal_hspec_mingw_flash_info_s flash_info;

	virtual void SetUp() {
		// Set flash info
		this->flash_info.page_count = 4;
		this->flash_info.pages = (mal_hspec_mingw_flash_page_info_s*)malloc(sizeof(mal_hspec_mingw_flash_page_info_s) * this->flash_info.page_count);
		this->flash_info.pages[0].size = this->first_page_size;
		this->flash_info.pages[1].size = this->second_page_size;
		this->flash_info.pages[2].size = this->first_page_size;
		this->flash_info.pages[3].size = this->second_page_size;
		mal_hspec_mingw_flash_set_flash(&this->flash_info);
	}

	virtual void TearDown() {
		free(this->flash_info.pages);
	}

};

TEST_F(TestMalE3prom, InitWithErasedFlash) {
	mal_error_e result;
	mal_e3prom_s e3prom;
	// Initialize e3prom
	mal_e3prom_init_s init;
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	result = mal_e3prom_init(&init, &e3prom);
	// Check result
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to initialize E3PROM";
	// Make sure primary section is active
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_PRIMARY) << "Primary section should be active";
	// Make sure primary section state is active
	uint32_t value;
	result = mal_e3prom_get_value(&e3prom, MAL_E3PROM_STATE_KEY, &value);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read primary state";
	ASSERT_EQ(value, MAL_E3PROM_STATE_ACTIVE) << "Primary section state should be active";
	// Make sure secondary state section is erased
	e3prom.active_section = MAL_E3PROM_SECTION_SECONDARY;
	result = mal_e3prom_get_value(&e3prom, MAL_E3PROM_STATE_KEY, &value);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read secondary state";
	ASSERT_EQ(value, MAL_E3PROM_STATE_ERASED) << "Secondary section state should be erased";
}

TEST_F(TestMalE3prom, InitWithPrimaryActive) {
	mal_error_e result;
	mal_e3prom_s e3prom;
	mal_e3prom_init_s init;
	// Initialize e3prom to get primary active
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	mal_e3prom_init(&init, &e3prom);
	// Now, the emulated flash is initialized. We will reinitialize to make
	// sure the e3prom handles initialized flash.
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	result = mal_e3prom_init(&init, &e3prom);
	// Check result
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to initialize E3PROM";
	// Make sure primary section is active
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_PRIMARY) << "Primary section should be active";
	// Make sure primary section state is active
	uint32_t value;
	result = mal_e3prom_get_value(&e3prom, MAL_E3PROM_STATE_KEY, &value);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read primary state";
	ASSERT_EQ(value, MAL_E3PROM_STATE_ACTIVE) << "Primary section state should be active";
}

TEST_F(TestMalE3prom, WriteAndRead) {
	mal_error_e result;
	mal_e3prom_s e3prom;
	mal_e3prom_init_s init;
	// Initialize e3prom to get primary active
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	mal_e3prom_init(&init, &e3prom);
	// We will write 4 different times
	for (uint32_t i = 0; i < 4; i++) {
		uint32_t test_value = 0x42 * i;
		// We will write 4 different keys
		result = mal_e3prom_write_value(&e3prom, 0, test_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
		result = mal_e3prom_write_value(&e3prom, 1, test_value + 1);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
		result = mal_e3prom_write_value(&e3prom, 2, test_value + 2);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
		result = mal_e3prom_write_value(&e3prom, 3, test_value + 3);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
		// We will read back values
		uint32_t read_value;
		result = mal_e3prom_get_value(&e3prom, 0, &read_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
		ASSERT_EQ(read_value, test_value) << "Read value mismatch";
		result = mal_e3prom_get_value(&e3prom, 1, &read_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
		ASSERT_EQ(read_value, test_value + 1) << "Read value mismatch";
		result = mal_e3prom_get_value(&e3prom, 2, &read_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
		ASSERT_EQ(read_value, test_value + 2) << "Read value mismatch";
		result = mal_e3prom_get_value(&e3prom, 3, &read_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
		ASSERT_EQ(read_value, test_value + 3) << "Read value mismatch";
	}
}

TEST_F(TestMalE3prom, SwitchSectionOnce) {
	mal_error_e result;
	mal_e3prom_s e3prom;
	mal_e3prom_init_s init;
	// Initialize e3prom to get primary active
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	mal_e3prom_init(&init, &e3prom);
	// Make sure primary section is active
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_PRIMARY) << "Primary section should be active";
	// Write enough times to switch sections
	uint32_t rom_size = (this->first_page_size + this->second_page_size) / (sizeof(uint32_t) * 2);
	for (uint32_t i = 0; i < rom_size; i++) {
		uint32_t read_value;
		result = mal_e3prom_write_value(&e3prom, 0, i);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
		result = mal_e3prom_get_value(&e3prom, 0, &read_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
		ASSERT_EQ(read_value, i) << "Read value mismatch";
	}
	// Make sure we switched to secondary section
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_SECONDARY) << "Secondary section should be active";
}

TEST_F(TestMalE3prom, SwitchSectionTwice) {
	mal_error_e result;
	mal_e3prom_s e3prom;
	mal_e3prom_init_s init;
	// Initialize e3prom to get primary active
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	mal_e3prom_init(&init, &e3prom);
	// Make sure primary section is active
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_PRIMARY) << "Primary section should be active";
	// Write enough times to switch sections
	uint32_t rom_size = (this->first_page_size + this->second_page_size) / (sizeof(uint32_t) * 2);
	for (uint32_t i = 0; i < rom_size; i++) {
		uint32_t read_value;
		result = mal_e3prom_write_value(&e3prom, 0, i);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
		result = mal_e3prom_get_value(&e3prom, 0, &read_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
		ASSERT_EQ(read_value, i) << "Read value mismatch";
	}
	// Make sure we switched to secondary section
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_SECONDARY) << "Secondary section should be active";
	// Write enough times to switch sections
	for (uint32_t i = 0; i < rom_size; i++) {
		uint32_t read_value;
		result = mal_e3prom_write_value(&e3prom, 0, i);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
		result = mal_e3prom_get_value(&e3prom, 0, &read_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
		ASSERT_EQ(read_value, i) << "Read value mismatch";
	}
	// Make sure we switched to primary section
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_PRIMARY) << "Secondary section should be active";
}

TEST_F(TestMalE3prom, InitWithSecondaryActive) {
	mal_error_e result;
	mal_e3prom_s e3prom;
	mal_e3prom_init_s init;
	// Initialize e3prom to get primary active
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	mal_e3prom_init(&init, &e3prom);
	// Make sure primary section is active
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_PRIMARY) << "Primary section should be active";
	// Write enough times to switch sections
	uint32_t rom_size = (this->first_page_size + this->second_page_size) / (sizeof(uint32_t) * 2);
	for (uint32_t i = 0; i < rom_size; i++) {
		uint32_t read_value;
		result = mal_e3prom_write_value(&e3prom, 0, i);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
		result = mal_e3prom_get_value(&e3prom, 0, &read_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
		ASSERT_EQ(read_value, i) << "Read value mismatch";
	}
	// Make sure we switched to secondary section
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_SECONDARY) << "Secondary section should be active";
	// Now, the emulated flash is initialized. We will reinitialize to make
	// sure the e3prom handles initialized flash.
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	result = mal_e3prom_init(&init, &e3prom);
	// Check result
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to initialize E3PROM";
	// Make sure secondary section is active
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_SECONDARY) << "Secondary section should be active";
	// Make sure secondary section state is active
	uint32_t value;
	result = mal_e3prom_get_value(&e3prom, MAL_E3PROM_STATE_KEY, &value);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read secondary state";
	ASSERT_EQ(value, MAL_E3PROM_STATE_ACTIVE) << "Secondary section state should be active";
}

TEST_F(TestMalE3prom, InitWithPrimaryDecommissioned) {
	mal_error_e result;
	mal_e3prom_s e3prom;
	mal_e3prom_init_s init;
	uint32_t value;
	// Initialize e3prom to get primary active
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	mal_e3prom_init(&init, &e3prom);
	// Make sure primary section is active
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_PRIMARY) << "Primary section should be active";
	// Write enough times to switch sections
	uint32_t rom_size = (this->first_page_size + this->second_page_size) / (sizeof(uint32_t) * 2);
	uint32_t last_primary_value;
	for (uint32_t i = 0; i < rom_size; i++) {
		uint32_t read_value;
		result = mal_e3prom_write_value(&e3prom, 0, i);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
		result = mal_e3prom_get_value(&e3prom, 0, &read_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
		ASSERT_EQ(read_value, i) << "Read value mismatch";
		// Check if section changed
		if (MAL_E3PROM_SECTION_PRIMARY != e3prom.active_section) {
			last_primary_value = i - 1;
			break;
		}
	}
	// Make sure we switched to secondary section
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_SECONDARY) << "Secondary section should be active";
	// Force state of secondary to init
	uint64_t key_address = mal_hspec_mingw_flash_get_page_start_address(3);
	key_address += mal_hspec_mingw_flash_get_page_size(3);
	key_address -= sizeof(uint32_t) * 2;
	uint32_t key = MAL_E3PROM_STATE_KEY;
	mal_hspec_mingw_flash_write_uint32_values(key_address, &key, 1);
	uint64_t value_address = key_address + sizeof(uint32_t);
	value = MAL_E3PROM_STATE_INITIALIZING;
	mal_hspec_mingw_flash_write_uint32_values(value_address, &value, 1);
	// Now, the emulated flash is initialized. We will reinitialize to make
	// sure the e3prom handles initialized flash.
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	result = mal_e3prom_init(&init, &e3prom);
	// Check result
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to initialize E3PROM";
	// Make sure secondary section is active
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_SECONDARY) << "Secondary section should be active";
	// Make sure secondary section state is active
	result = mal_e3prom_get_value(&e3prom, MAL_E3PROM_STATE_KEY, &value);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read secondary state";
	ASSERT_EQ(value, MAL_E3PROM_STATE_ACTIVE) << "Secondary section state should be active";
	// Read value to make sure it got transfered
	result = mal_e3prom_get_value(&e3prom, 0, &value);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read value";
	ASSERT_EQ(value, last_primary_value) << "Value mismatch when restoring primary decommissioned";
}

TEST_F(TestMalE3prom, InitWithSecondaryDecommissioned) {
	mal_error_e result;
	mal_e3prom_s e3prom;
	mal_e3prom_init_s init;
	uint32_t value;
	// Initialize e3prom to get primary active
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	mal_e3prom_init(&init, &e3prom);
	// Make sure primary section is active
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_PRIMARY) << "Primary section should be active";
	// Write enough times to switch sections
	uint32_t rom_size = (this->first_page_size + this->second_page_size) / (sizeof(uint32_t) * 2);
	for (uint32_t i = 0; i < rom_size; i++) {
		uint32_t read_value;
		result = mal_e3prom_write_value(&e3prom, 0, i);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
		result = mal_e3prom_get_value(&e3prom, 0, &read_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
		ASSERT_EQ(read_value, i) << "Read value mismatch";
	}
	// Make sure we switched to secondary section
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_SECONDARY) << "Secondary section should be active";
	// Write enough times to switch sections
	uint32_t last_secondary_value;
	for (uint32_t i = 0; i < rom_size; i++) {
		uint32_t read_value;
		result = mal_e3prom_write_value(&e3prom, 0, i);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to write to E3PROM";
		result = mal_e3prom_get_value(&e3prom, 0, &read_value);
		ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read from E3PROM";
		ASSERT_EQ(read_value, i) << "Read value mismatch";
		// Check if section changed
		if (MAL_E3PROM_SECTION_SECONDARY != e3prom.active_section) {
			last_secondary_value = i - 1;
			break;
		}
	}
	// Make sure we switched to primary section
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_PRIMARY) << "Primary section should be active";
	// Force state of primary to init
	uint64_t key_address = mal_hspec_mingw_flash_get_page_start_address(1);
	key_address += mal_hspec_mingw_flash_get_page_size(1);
	key_address -= sizeof(uint32_t) * 2;
	uint32_t key = MAL_E3PROM_STATE_KEY;
	mal_hspec_mingw_flash_write_uint32_values(key_address, &key, 1);
	uint64_t value_address = key_address + sizeof(uint32_t);
	value = MAL_E3PROM_STATE_INITIALIZING;
	mal_hspec_mingw_flash_write_uint32_values(value_address, &value, 1);
	// Now, the emulated flash is initialized. We will reinitialize to make
	// sure the e3prom handles initialized flash.
	init.primary_start_page = 0;
	init.primary_page_count = 2;
	init.secondary_start_page = 2;
	init.secondary_page_count = 2;
	result = mal_e3prom_init(&init, &e3prom);
	// Check result
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to initialize E3PROM";
	// Make sure primary section is active
	ASSERT_EQ(e3prom.active_section, MAL_E3PROM_SECTION_PRIMARY) << "Primary section should be active";
	// Make sure primary section state is active
	result = mal_e3prom_get_value(&e3prom, MAL_E3PROM_STATE_KEY, &value);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read primary state";
	ASSERT_EQ(value, MAL_E3PROM_STATE_ACTIVE) << "Primary section state should be active";
	// Read value to make sure it got transfered
	result = mal_e3prom_get_value(&e3prom, 0, &value);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to read value";
	ASSERT_EQ(value, last_secondary_value) << "Value mismatch when restoring secondary decommissioned";
}
