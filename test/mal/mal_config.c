/*
 * mal_config.c
 *
 *  Created on: May 19, 2015
 *      Author: Olivier
 */

#include "hspec/mal_hspec_cmn.h"

const mal_hspec_system_clk_s mal_target_system_clock = {
		.frequency = MAL_TARGET_SYSTEM_CLOCK,
		.src = MAL_TARGET_SYSTEM_CLOCK_SRC
};

uint64_t mal_external_clk_freq = MAL_EXTERNAL_CLOCK_FREQUENCY;

float mal_external_vdda = 3.3f;
