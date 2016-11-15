/**
 * @file mal_dac.h
 * @author Olivier Allaire
 * @date October 25 2016
 * @copyright Copyright (c) 2015 Olivier Allaire
 * @par This file is part of MAL.
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
 * @brief Functions to access and configure the DACs of the MCU.
 */

#ifndef DAC_MAL_DAC_H_
#define DAC_MAL_DAC_H_

#include "hspec/mal_hspec.h"

/**
 * @defgroup DAC
 * @brief @copybrief mal_dac.h
 * @{
 */

/**
 * @brief Write to a DAC.
 * @param dac The DAC to write to. Should be of type ::mal_hspec_dac_e.
 * @param value A value of type uint64_t that that contains the data to write.
 * @return Returns #MAL_ERROR_OK on success.
 */
#define mal_dac_write_bits(dac, value) mal_hspec_dac_write(dac, value)

/**
 * @brief Get the resolution of the DAC.
 * @param dac The DAC to get the resolution from.
 * @param resolution A pointer of type uint8_t that will contain the
 * resolution.
 * @return Returns #MAL_ERROR_OK on success.
 */
#define mal_dac_resolution(dac, resolution) mal_hspec_dac_resolution(dac, resolution)

/**
 * @brief Initialize a DAC.
 * @param init The initialization parameters.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_dac_init(mal_hspec_dac_init_s *init);

/**
 * @brief Write to a DAC, but as volts instead of LSBs.
 * @param dac The DAC to write to.
 * @param value The value to write.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_dac_write_volts(mal_hspec_dac_e dac, float value);

/**
 * @brief Convert a bit value of a DAC to volts.
 * @param dac The DAC to which the bit value will be written to.
 * @param value The value to write.
 * @param bit_value The converted value.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_dac_volts_to_bits(mal_hspec_dac_e dac, float value, uint64_t *bit_value);

/**
 * @}
 */

#endif /* DAC_MAL_DAC_H_ */
