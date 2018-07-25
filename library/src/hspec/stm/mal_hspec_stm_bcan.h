/*
 * Copyright (c) 2018 Olivier Allaire
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

#ifndef MAL_MAL_HSPEC_STM_BCAN_H
#define MAL_MAL_HSPEC_STM_BCAN_H

#include "std/mal_error.h"
#include "std/mal_stdint.h"
#include "can/mal_can_definitions.h"
#include "std/mal_bool.h"

#define MAL_HSPEC_STM_BCAN_FILTER_BANKS_SIZE	14
#define MAL_HSPEC_STM_BCAN_FILTER_STD_SIZE		2

typedef struct {
    uint32_t id;
    uint32_t mask;
} mal_hspec_stm_bcan_extended_filter_s;

typedef struct {
    uint16_t id[MAL_HSPEC_STM_BCAN_FILTER_STD_SIZE];
    uint16_t mask[MAL_HSPEC_STM_BCAN_FILTER_STD_SIZE];
} mal_hspec_stm_bcan_standard_filter_s;

typedef union {
    mal_hspec_stm_bcan_extended_filter_s ext;
    mal_hspec_stm_bcan_standard_filter_s std;
} mal_hspec_stm_bcan_filter_u;

typedef enum {
    MAL_HSPEC_STM_BCAN_FIFO_0,
    MAL_HSPEC_STM_BCAN_FIFO_1
} mal_hspec_stm_bcan_fifo_e;

typedef struct {
    mal_hspec_stm_bcan_filter_u filter;
    uint8_t filter_count;
    mal_can_id_type_e type;
    bool is_active;
    mal_hspec_stm_bcan_fifo_e fifo;
} mal_hspec_stm_bcan_filter_bank_s;

typedef struct {
    mal_hspec_stm_bcan_filter_bank_s filter_banks[MAL_HSPEC_STM_BCAN_FILTER_BANKS_SIZE];
    mal_hspec_stm_bcan_fifo_e active_fifo;
} mal_hspec_stm_bcan_filter_banks_s;

void mal_hspec_stm_bcan_init_filter_banks(mal_hspec_stm_bcan_filter_banks_s *handle);

mal_error_e mal_hspec_stm_bcan_add_filter(mal_hspec_stm_bcan_filter_banks_s *handle, mal_can_filter_s *filter,
                                          uint8_t *filter_index);

bool mal_hspec_stm_bcan_remove_filter(mal_hspec_stm_bcan_filter_banks_s *handle, mal_can_filter_s *filter,
                                      uint8_t *filter_index);

mal_error_e mal_hspec_stm_bcan_get_bit_timing(uint64_t bitrate, uint64_t pclk, uint32_t *prescaler, uint32_t *tseg1,
                                              int32_t *tseg2, uint32_t *sjw);

#endif //MAL_MAL_HSPEC_STM_BCAN_H
