/*
 * test_mal_timer.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: Olivier
 */

extern "C" {
#include "startup/mal_startup.h"
#include "timer/mal_timer.h"
}

#include "test_mal_timer.h"

void TestMalTimer::SetUp() {
	// Set up MAL library
	mal_startup_hardware();
}

void TestMalTimer::TearDown() {
}

TEST_F(TestMalTimer, SubCounts) {
	// Test 8 bit wraparound
	uint64_t count1 = 254;
	uint64_t count2 = 3;
	uint64_t mask = 0xFF;
	uint64_t result = MAL_TIMER_SUB_COUNTS(count2, count1, mask);
	ASSERT_EQ(result, 5) << "Failed to subtract 8 bit values with wrap around";
	// Test 8 bit normal
	count1 = 100;
	count2 = 200;
	result = MAL_TIMER_SUB_COUNTS(count2, count1, mask);
	ASSERT_EQ(result, 100) << "Failed to subtract 8 bit values";
	// Test 16 bit wraparound
	count1 = 65534;
	count2 = 3;
	mask = 0xFFFF;
	result = MAL_TIMER_SUB_COUNTS(count2, count1, mask);
	ASSERT_EQ(result, 5) << "Failed to subtract 16 bit values with wrap around";
	// Test 16 bit normal
	count1 = 1100;
	count2 = 1200;
	result = MAL_TIMER_SUB_COUNTS(count2, count1, mask);
	ASSERT_EQ(result, 100) << "Failed to subtract 16 bit values";
	// Test 32 bit wraparound
	count1 = 4294967294UL;
	count2 = 3;
	mask = 0xFFFFFFFF;
	result = MAL_TIMER_SUB_COUNTS(count2, count1, mask);
	ASSERT_EQ(result, 5) << "Failed to subtract 32 bit values with wrap around";
	// Test 32 bit normal
	count1 = 1000100;
	count2 = 1000200;
	result = MAL_TIMER_SUB_COUNTS(count2, count1, mask);
	ASSERT_EQ(result, 100) << "Failed to subtract 32 bit values";
	// Test 64 bit wraparound
	count1 = 18446744073709551614ULL;
	count2 = 3;
	mask = 0xFFFFFFFFFFFFFFFFULL;
	result = MAL_TIMER_SUB_COUNTS(count2, count1, mask);
	ASSERT_EQ(result, 5) << "Failed to subtract 64 bit values with wrap around";
	// Test 64 bit normal
	count1 = 1000000000100ULL;
	count2 = 1000000000200ULL;
	result = MAL_TIMER_SUB_COUNTS(count2, count1, mask);
	ASSERT_EQ(result, 100) << "Failed to subtract 64 bit values";
}
