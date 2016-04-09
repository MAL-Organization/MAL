/*
 * test_mal_timer.h
 *
 *  Created on: Apr 8, 2016
 *      Author: Olivier
 */

#ifndef TIMER_TEST_MAL_TIMER_H_
#define TIMER_TEST_MAL_TIMER_H_

#include "gtest/gtest.h"

class TestMalTimer: public ::testing::Test {
protected:
	virtual void SetUp();
	virtual void TearDown();
};

#endif /* TIMER_TEST_MAL_TIMER_H_ */
