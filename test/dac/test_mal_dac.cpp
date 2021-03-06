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
#include "hspec/gnu/mal_hspec_gnu_power.h"
}

#include "gtest/gtest.h"

class TestMalDac : public ::testing::Test {

};

TEST_F(TestMalDac, VoltsToBits) {
	mal_error_e result;
	mal_dac_s handle;
	// Setup power rail
	mal_volts_t	vdda = MAL_TYPES_VOLTS_TO_MAL_VOLTS(3.3f);
	mal_hspec_gnu_power_set_rail_voltage(MAL_POWER_RAIL_VDDA, vdda);
	// Test data
	mal_dac_e test_dac = MAL_DAC_0;
	uint8_t test_resolution = 16;
	// Setup DAC
	mal_dac_init_s init;
	init.dac = test_dac;
	init.port = MAL_GPIO_PORT_A;
	init.pin = 0;
	init.bit_resolution = test_resolution;
	result = mal_dac_init(&init, &handle);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to initialize DAC";
	// Test with half of maximum value
	uint64_t conversion_result;
	result = mal_dac_volts_to_bits(&handle, vdda / 2, &conversion_result);
	ASSERT_EQ(result, MAL_ERROR_OK) << "Failed to convert value";
	ASSERT_EQ(conversion_result, 32767);
}
