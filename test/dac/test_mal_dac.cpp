/*
 * test_mal_dac.cpp
 *
 *  Created on: Oct 25, 2016
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
#include "dac/mal_dac.h"
#include "hspec/mingw/mal_hspec_mingw_dac.h"

extern float mal_external_vdda;
}

#include "gtest/gtest.h"

class TestMalDac : public ::testing::Test {

};

TEST_F(TestMalDac, VoltsToBits) {
	mal_error_e result;
	// Test data
	mal_hspec_dac_e test_dac = MAL_HSPEC_DAC_0;
	uint8_t test_resolution = 16;
	mal_hspec_gpio_s test_gpio;
	test_gpio.port = MAL_HSPEC_GPIO_PORT_A;
	test_gpio.pin = 0;
	// Setup DAC
	mal_hspec_dac_init_s init;
	init.dac = test_dac;
	init.gpio = &test_gpio;
	init.bit_resolution = test_resolution;
	result = mal_dac_init(&init);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to initialize DAC";
	// Test with half of maximum value
	uint64_t conversion_result;
	result = mal_dac_volts_to_bits(test_dac, mal_external_vdda / 2, &conversion_result);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to convert value";
	ASSERT_EQ(conversion_result, 32767);
}
