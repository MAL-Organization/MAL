/*
 * test_mal_timer.h
 *
 *  Created on: Apr 8, 2016
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

#ifndef TIMER_TEST_MAL_TIMER_H_
#define TIMER_TEST_MAL_TIMER_H_

extern "C" {
#include "timer/mal_timer.h"
}

#include "gtest/gtest.h"

class TestMalTimer: public ::testing::Test {
protected:
	static const mal_timer_e test_timer = MAL_TIMER_1;

	mal_timer_s handle;

	virtual void SetUp();
	virtual void TearDown();
};

#endif /* TIMER_TEST_MAL_TIMER_H_ */
