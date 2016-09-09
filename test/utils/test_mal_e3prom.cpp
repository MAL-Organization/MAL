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
