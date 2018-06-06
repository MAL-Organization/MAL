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

#include "gpio/mal_gpio_definitions.h"
#include "std/mal_stdint.h"
#include "std/mal_error.h"
#include "std/mal_bool.h"
#include "std/mal_types.h"
#include "std/mal_defs.h"

/**
 * @defgroup ADC
 * @brief @copybrief mal_adc.h
 * @{
 */

/**
* ADC handle that must be defined by hardware specific implementation. Used
* to access the adc functions.
*/
typedef struct MAL_ADC mal_adc_s;

/**
 * The possible ADCs.
 */
typedef enum {
	MAL_ADC_0 = 0,   //!< MAL_ADC_0
	MAL_ADC_1 = 1,   //!< MAL_ADC_1
	MAL_ADC_2 = 2,   //!< MAL_ADC_2
	MAL_ADC_3 = 3,   //!< MAL_ADC_3
	MAL_ADC_4 = 4,   //!< MAL_ADC_4
	MAL_ADC_5 = 5,   //!< MAL_ADC_5
	MAL_ADC_6 = 6,   //!< MAL_ADC_6
	MAL_ADC_7 = 7,   //!< MAL_ADC_7
	MAL_ADC_8 = 8,   //!< MAL_ADC_8
	MAL_ADC_9 = 9,   //!< MAL_ADC_9
	MAL_ADC_10 = 10, //!< MAL_ADC_10
	MAL_ADC_11 = 11, //!< MAL_ADC_11
	MAL_ADC_12 = 12, //!< MAL_ADC_12
	MAL_ADC_13 = 13, //!< MAL_ADC_13
	MAL_ADC_14 = 14, //!< MAL_ADC_14
	MAL_ADC_15 = 15, //!< MAL_ADC_15
	MAL_ADC_SIZE = 16//!< MAL_ADC_SIZE
} mal_adc_e;

/**
 * The initialization parameters of an ADC.
 */
typedef struct {
	mal_adc_e adc; /**< To ADC to initialize.*/
	mal_gpio_port_e port; /**< The port of the ADC GPIO to initialize.*/
	uint8_t pin; /**< The pin of the port of the ADC GPIO to initialize. */
	uint8_t bit_resolution; /**< The resolution of the ADC.*/
} mal_adc_init_s;

/**
 * @brief This callback will be executed when an ADC read is complete.
 * @param handle The handle given for the callback.
 * @param value The value read.
 * @return Return a status once you executed your callback. For now, nothing is
 * done with this status.
 */
typedef mal_error_e (*mal_adc_read_callback_t)(void *handle, uint64_t value);

/**
 * @brief Read an ADC.
 * @param handle The ADC handle to read from.
 * @param value A pointer of type uint64_t that that will contain the data
 * read.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_read_bits(mal_adc_s *handle, uint64_t *value);

/**
 * @brief Get the resolution of the ADC.
 * @param handle The ADC to get the resolution from.
 * @param resolution A pointer of type uint8_t that will contain the
 * resolution.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_resolution(mal_adc_s *handle, uint8_t *resolution);

/**
 * @brief Start an async read. A callback will be executed once the read is
 * complete.
 * @param handle The ADC to read from.
 * @param callback The callback to execute once the read is complete.
 * @param callback_handle This handle will be passed to the callback funtion.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_async_read(mal_adc_s *handle, mal_adc_read_callback_t callback, void *callback_handle);

/**
 * @brief Disable interrupts for an ADC.
 * @param handle The ADC to disable the interrupt.
 * @return Returns true if interrupt was active before disabling it.
 */
MAL_DEFS_INLINE bool mal_adc_disable_interrupt(mal_adc_s *handle);

/**
 * @brief Enable interrupts for an ADC.
 * @param handle The ADC to enable the interrupt.
 * @param active A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
MAL_DEFS_INLINE void mal_adc_set_interrupt(mal_adc_s *handle, bool active);

/**
 * @brief Get the maximum resolution of the ADC.
 * @param adc The ADC to get the resolution from.
 * @param resolution A pointer of type uint8_t that will contain the
 * resolution.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_maximum_resolution(mal_adc_e adc, uint8_t *resolution);

/**
 * @brief Initialize an ADC.
 * @param init The initialization parameters.
 * @param handle The handle to initialize. This handle is used to access
 * subsequent functions.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_init(mal_adc_init_s *init, mal_adc_s *handle);

/**
 * @brief Read an ADC values, but as volts instead of LSBs.
 * @param handle The ADC to read from.
 * @param value The value read.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_read_volts(mal_adc_s *handle, mal_volts_t *value);

/**
 * @brief Convert a bit value of an ADC to volts.
 * @param handle The ADC from which the bit value was read from.
 * @param bit_value The value read rom the ADC.
 * @param value The converted value.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_bits_to_volts(mal_adc_s *handle, uint64_t bit_value, mal_volts_t *value);

/**
 * @}
 */

#endif /* ADC_MAL_ADC_H_ */
