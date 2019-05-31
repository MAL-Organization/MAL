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
#include "ram/mal_ram.h"
#include "hspec/gnu/mal_hspec_gnu_ram.h"
}

#include "gtest/gtest.h"

class TestMalRam : public ::testing::Test {
protected:
    static const unsigned int ram_size = 100;

    void SetUp();
    void TearDown();
};

const unsigned int TestMalRam::ram_size;

void TestMalRam::SetUp() {
    mal_hspec_gnu_ram_stack_start = 0x20000000;
    mal_hspec_gnu_ram_stack_end = mal_hspec_gnu_ram_stack_start + this->ram_size - 1;
    mal_hspec_gnu_ram_stack_pointer_address = mal_hspec_gnu_ram_stack_end + 1;
    mal_hspec_gnu_ram_simulated_ram = (uint8_t*)malloc(this->ram_size);
    mal_ram_init_stack_monitor();
}

void TestMalRam::TearDown() {
    free(mal_hspec_gnu_ram_simulated_ram);
}

TEST_F(TestMalRam, InitStackMonitor) {
    for (unsigned int i = 0; i < this->ram_size; i++) {
        ASSERT_EQ(mal_hspec_gnu_ram_simulated_ram[i], MAL_RAM_STACK_MARKER) << "Address " << i;
    }
}

TEST_F(TestMalRam, GetStackSize) {
    ASSERT_EQ(mal_ram_get_stack_size(), this->ram_size);
}

TEST_F(TestMalRam, GetStackUsage) {
    // Initial call should be 0%
    mal_ratio_t value;
    value = mal_ram_get_stack_usage();
    ASSERT_EQ(MAL_TYPES_RATIO_OF_INT_VALUE(value, uint8_t, 100), 0U);
    // Use 10%
    for (unsigned int i = 0; i < (this->ram_size / 10); i++) {
        mal_hspec_gnu_ram_simulated_ram[this->ram_size - i - 1] = MAL_RAM_STACK_MARKER + 1;
    }
    value = mal_ram_get_stack_usage();
    ASSERT_EQ(MAL_TYPES_RATIO_OF_INT_VALUE(value, uint8_t, 100), 10U);
    // Use 50%
    for (unsigned int i = 0; i < (this->ram_size / 2); i++) {
        mal_hspec_gnu_ram_simulated_ram[this->ram_size - i - 1] = MAL_RAM_STACK_MARKER + 1;
    }
    value = mal_ram_get_stack_usage();
    ASSERT_EQ(MAL_TYPES_RATIO_OF_INT_VALUE(value, uint8_t, 100), 50U);
    // Use 100%
    for (unsigned int i = 0; i < this->ram_size; i++) {
        mal_hspec_gnu_ram_simulated_ram[this->ram_size - i - 1] = MAL_RAM_STACK_MARKER + 1;
    }
    value = mal_ram_get_stack_usage();
    ASSERT_EQ(MAL_TYPES_RATIO_OF_INT_VALUE(value, uint8_t, 100), 100U);
}
