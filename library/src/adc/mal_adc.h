/**
 * @file mal_adc.h
 * @author Olivier Allaire
 * @date September 11 2015
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
 * @brief Functions to access and configure the ADCs of the MCU.
 */


#ifndef ADC_MAL_ADC_H_
#define ADC_MAL_ADC_H_

#include "hspec/mal_hspec.h"

/**
 * @defgroup ADC
 * @brief @copybrief mal_can.h
 * @{
 */

/**
 * @brief Read an ADC.
 * @param adc The ADC to read from. Should be of type ::mal_hspec_adc_e.
 * @param value A pointer of type uint64_t that that will contain the data
 * read.
 * @return Returns #MAL_ERROR_OK on success.
 */
#define mal_adc_read_bits(adc, value) mal_hspec_adc_read(adc, value)

/**
 * @brief Get the resolution of the ADC.
 * @param adc The ADC to get the resolution from.
 * @param resolution A pointer of type uint8_t that will contain the
 * resolution.
 * @return Returns #MAL_ERROR_OK on success.
 */
#define mal_adc_resolution(adc, resolution) mal_hspec_adc_resolution(adc, resolution)

/**
 * @brief Start an async read. A callback will be executed once the read is
 * complete.
 * @param adc The ADC to read from. Should be of type ::mal_hspec_adc_e.
 * @param callback The callback to execute once the read is complete. Should be
 * of type ::mal_hspec_adc_read_callback_t.
 * @return Returns #MAL_ERROR_OK on success.
 */
#define mal_adc_async_read(adc, callback) mal_hspec_adc_async_read(adc, callback)

/**
 * @brief Disable interrupts for an ADC.
 * @param adc The ADC to disable the interrupt. Should be of type
 * ::mal_hspec_adc_e.
 * @return Returns true if interrupt was active before disabling it.
 */
#define mal_adc_disable_interrupt(adc) mal_hspec_disable_adc_interrupt(adc)

/**
 * @brief Enable interrupts for an ADC.
 * @param adc The ADC to enable the interrupt.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
#define mal_adc_enable_interrupt(adc, active) mal_hspec_enable_adc_interrupt(adc, active)

/**
 * @brief Initialise an ADC.
 * @param init The initialisation parameters.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_init(mal_hspec_adc_init_s *init);

/**
 * @brief Read an ADC values, but as volts instead of LSBs.
 * @param adc The ADC to read from.
 * @param value The value read.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_read_volts(mal_hspec_adc_e adc, float *value);

/**
 * @brief Convert a bit value of an ADC to volts.
 * @param adc The ADC from which the bit value was read from.
 * @param bit_value The value read rom the ADC.
 * @param value The converted value.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_bits_to_volts(mal_hspec_adc_e adc, uint64_t bit_value, float *value);

/**
 * @}
 */

#endif /* ADC_MAL_ADC_H_ */
