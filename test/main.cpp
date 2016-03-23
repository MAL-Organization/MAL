/*
 * AllTests.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: Olivier
 */

#include "gtest/gtest.h"

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

