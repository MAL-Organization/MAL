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

#include "mal_hspec_stm_bcan.h"
#include "std/mal_stdlib.h"

static uint32_t mal_hspec_stm_bcan_extended_fr_format(uint32_t id);
static uint16_t mal_hspec_stm_bcan_standard_fr_format(uint16_t id);

void mal_hspec_stm_bcan_init_filter_banks(mal_hspec_stm_bcan_filter_banks_s *handle) {
    handle->active_fifo = MAL_HSPEC_STM_BCAN_FIFO_0;
    uint8_t  i;
    for (i = 0; i < MAL_HSPEC_STM_BCAN_FILTER_BANKS_SIZE; i++) {
        handle->filter_banks[i].is_active = false;
    }
}

mal_error_e mal_hspec_stm_bcan_get_bit_timing(uint64_t bitrate, uint64_t pclk, uint32_t *prescaler, uint32_t *tseg1,
                                              int32_t *tseg2, uint32_t *sjw) {
    // Compute bit time
    bool done = false;
    for (*prescaler = 1; *prescaler <= 1024; (*prescaler)++) {
        // Calculate total time quantas
        uint32_t tq_total = (uint32_t )(pclk / (*prescaler * bitrate));
        if (!tq_total) {
            continue;
        }
        // Remove tq to account for sync segment.
        tq_total--;
        // TSEG1 must be at least 2 time quantas long because of it includes
        // the propagation segment which takes 1 time quanta.
        for (*tseg1 = 2; *tseg1 < 16; (*tseg1)++) {
            *tseg2 = tq_total - *tseg1;
            // Here are the rules to pass this point.
            // 1. TSEG2 must be equal or less to TSEG1. This is to ensure that
            //    the sample point is not before the 50% mark.
            if (*tseg2 > *tseg1) {
                continue;
            }
            // 2. Since TSEG2 must have a time, it cannot be 0.
            if (*tseg2 <= 0) {
                continue;
            }
            // 3. TSEG2 has a maximum of 8 time quantas.
            if (*tseg2 > 8) {
                continue;
            }
            // Limit error to 0.5%
            int64_t int_error = ((int64_t)pclk * (int64_t)1000) / (bitrate * (int64_t)*prescaler * (int64_t)(1 + *tseg1 + *tseg2));
            int_error = abs((int)(1000 - int_error));
            if (int_error > 5) {
                continue;
            }
            // Now we have to find a suitable synchronisation jump width.
            for (*sjw = 4; *sjw >= 1; (*sjw)--) {
                // Jump must not be longer than TSEG2 because the jump lengthen
                // or shorten TSEG2.
                if (*sjw < *tseg2) {
                    done = true;
                    break;
                }
            }
            if (done) {
                break;
            }
        }
        if (done) {
            break;
        }
    }
    if (!done) {
        return MAL_ERROR_CLOCK_ERROR;
    }
    *tseg1 -= 1;
    *tseg2 -= 1;
    *sjw -= 1;

    return MAL_ERROR_OK;
}

mal_error_e mal_hspec_stm_bcan_add_filter(mal_hspec_stm_bcan_filter_banks_s *handle, mal_can_filter_s *filter,
                                          uint8_t *filter_index) {
    // Find a free filter
    bool found = false;
    for (*filter_index = 0; *filter_index < MAL_HSPEC_STM_BCAN_FILTER_BANKS_SIZE; (*filter_index)++) {
        // Check if filter is unused.
        if (!handle->filter_banks[*filter_index].is_active) {
            found = true;
            break;
        }
        // If the ID is standard, it is possible to resuse that filter if it
        // has an available slot
        if (MAL_CAN_ID_STANDARD == filter->id_type && MAL_CAN_ID_STANDARD == handle->filter_banks[*filter_index].type) {
            if (handle->filter_banks[*filter_index].filter_count < MAL_HSPEC_STM_BCAN_FILTER_STD_SIZE) {
                found = true;
                break;
            }
        }
    }
    if (!found) {
        return MAL_ERROR_HARDWARE_UNAVAILABLE;
    }
    // Initialise filter array
    if (MAL_CAN_ID_EXTENDED == filter->id_type) {
        handle->filter_banks[*filter_index].is_active = true;
        handle->filter_banks[*filter_index].type = MAL_CAN_ID_EXTENDED;
        handle->filter_banks[*filter_index].filter.ext.id = mal_hspec_stm_bcan_extended_fr_format(filter->id);
        handle->filter_banks[*filter_index].filter.ext.mask = mal_hspec_stm_bcan_extended_fr_format(filter->mask);
    } else {
        // Check if filter is already active to reset count at the same time.
        if (!handle->filter_banks[*filter_index].is_active) {
            handle->filter_banks[*filter_index].filter_count = 0;
            handle->filter_banks[*filter_index].is_active = true;
        }
        // Set correct mask and filter
        handle->filter_banks[*filter_index].filter.std.id[handle->filter_banks[*filter_index].filter_count] = mal_hspec_stm_bcan_standard_fr_format((uint16_t)filter->id);
        handle->filter_banks[*filter_index].filter.std.mask[handle->filter_banks[*filter_index].filter_count] = mal_hspec_stm_bcan_standard_fr_format((uint16_t)filter->mask);
        handle->filter_banks[*filter_index].filter_count++;
        // Fill remaining filters with the last one
        uint32_t i;
        for (i = handle->filter_banks[*filter_index].filter_count; i < MAL_HSPEC_STM_BCAN_FILTER_STD_SIZE; i++) {
            handle->filter_banks[*filter_index].filter.std.id[i] = mal_hspec_stm_bcan_standard_fr_format((uint16_t)filter->id);
            handle->filter_banks[*filter_index].filter.std.mask[i] = mal_hspec_stm_bcan_standard_fr_format((uint16_t)filter->mask);
        }
    }
    // Set and switch fifo
    handle->filter_banks[*filter_index].fifo = handle->active_fifo;
    if (MAL_HSPEC_STM_BCAN_FIFO_0 == handle->active_fifo) {
        handle->active_fifo = MAL_HSPEC_STM_BCAN_FIFO_1;
    } else {
        handle->active_fifo = MAL_HSPEC_STM_BCAN_FIFO_0;
    }

    return MAL_ERROR_OK;
}

bool mal_hspec_stm_bcan_remove_filter(mal_hspec_stm_bcan_filter_banks_s *handle, mal_can_filter_s *filter,
                                      uint8_t *filter_index) {
    // Format id and mask
    if (MAL_CAN_ID_STANDARD == filter->id_type) {
        filter->id = mal_hspec_stm_bcan_standard_fr_format((uint16_t)filter->id);
        filter->mask = mal_hspec_stm_bcan_standard_fr_format((uint16_t)filter->mask);
    } else {
        filter->id = mal_hspec_stm_bcan_extended_fr_format(filter->id);
        filter->mask = mal_hspec_stm_bcan_extended_fr_format(filter->mask);
    }
    // Find filter index
    uint8_t std_filter_index = 0;
    bool found = false;
    for (*filter_index = 0; *filter_index < MAL_HSPEC_STM_BCAN_FILTER_BANKS_SIZE; (*filter_index)++) {
        // Check if filter is unused.
        if (!handle->filter_banks[*filter_index].is_active) {
            continue;
        }

        if (MAL_CAN_ID_STANDARD == filter->id_type && MAL_CAN_ID_STANDARD == handle->filter_banks[*filter_index].type) {
            int32_t i;
            for (i = handle->filter_banks[*filter_index].filter_count - 1; i >=0; i--) {
                if (handle->filter_banks[*filter_index].filter.std.id[i] == filter->id && handle->filter_banks[*filter_index].filter.std.mask[i] == filter->mask) {
                    std_filter_index = (uint8_t)i;
                    found = true;
                    break;
                }
            }
        } else if (MAL_CAN_ID_EXTENDED == filter->id_type && MAL_CAN_ID_EXTENDED == handle->filter_banks[*filter_index].type) {
            if (handle->filter_banks[*filter_index].filter.ext.id == filter->id && handle->filter_banks[*filter_index].filter.ext.mask == filter->mask) {
                found = true;
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        return false;
    }
    // Free filter array, especially for standard type
    if (MAL_CAN_ID_EXTENDED == filter->id_type) {
        handle->filter_banks[*filter_index].is_active = false;
    } else {
        if (handle->filter_banks[*filter_index].filter_count <= 1) {
            handle->filter_banks[*filter_index].is_active = false;
        } else {
            uint8_t copy_index = 0;
            if (std_filter_index == 0) {
                copy_index = 1;
            }
            // Set correct mask and filter
            handle->filter_banks[*filter_index].filter.std.id[std_filter_index] = handle->filter_banks[*filter_index].filter.std.id[copy_index];
            handle->filter_banks[*filter_index].filter.std.mask[std_filter_index] = handle->filter_banks[*filter_index].filter.std.mask[copy_index];
            handle->filter_banks[*filter_index].filter_count--;
        }
    }
    return true;
}

static uint32_t mal_hspec_stm_bcan_extended_fr_format(uint32_t id) {
    uint32_t result = 0;
    // 11 MSb are the 11 bits of the standard ID (first 11 bits of
    // extended ID)
    result |= (id & 0x3FF) << 21;
    // Next 18 bits is the MS part of the extended ID
    result |= (id & 0x1FFFF800) << 3;
    // Next and last 3 bits are flags. IDE which is set since this is
    // extended. RTR not set since we don't support remote frames. And
    // the last one must be 0.
    result |= 4;

    return result;
}

static uint16_t mal_hspec_stm_bcan_standard_fr_format(uint16_t id) {
    uint16_t result = 0;
    // 11 MSb are the 11 bits of the standard ID (first 11 bits of
    // extended ID)
    result |= (id & 0x3FF) << 5;
    // Next and last 5 bits are flags. RTR which is not set since we don't
    // support remote frames. IDE which is not set since this is standard. And
    // the last 3 must be 0 since they are only valid in extended.
    result |= 0;

    return result;
}
