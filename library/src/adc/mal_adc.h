/**
 * @file mal_adc.h
 * @author Olivier Allaire
 * @date October 24 2018
 * @copyright Copyright (c) 2018 Olivier Allaire
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
* ADC handle that must be defined by hardware specific implementation. Used to access the ADC functions.
*/
typedef struct MAL_ADC mal_adc_s;

/**
* ADC channel handle that must be defined by hardware specific implementation. Used to access the ADC channel functions.
*/
typedef struct MAL_ADC_CHANNEL mal_adc_channel_s;

/**
 * This structure is used to retain interrupt status between disable and
 * restore. Must be defined by the hardware specific implementation.
 */
typedef struct MAL_ADC_INTERRUPT_STATE mal_adc_interrupt_state_s;

/**
 * The possible ADCs.
 */
typedef enum {
	MAL_ADC_0 = 0,   //!< MAL_ADC_0
	MAL_ADC_1 = 1,   //!< MAL_ADC_1
	MAL_ADC_2 = 2,   //!< MAL_ADC_2
    MAL_ADC_SIZE = 3
} mal_adc_e;

/**
 * The possible channels within an ADC
 */
typedef enum {
    MAL_ADC_CHANNEL_0 = 0,
    MAL_ADC_CHANNEL_1 = 1,
    MAL_ADC_CHANNEL_2 = 2,
    MAL_ADC_CHANNEL_3 = 3,
    MAL_ADC_CHANNEL_4 = 4,
    MAL_ADC_CHANNEL_5 = 5,
    MAL_ADC_CHANNEL_6 = 6,
    MAL_ADC_CHANNEL_7 = 7,
    MAL_ADC_CHANNEL_8 = 8,
    MAL_ADC_CHANNEL_9 = 9,
    MAL_ADC_CHANNEL_10 = 10,
    MAL_ADC_CHANNEL_11 = 11,
    MAL_ADC_CHANNEL_12 = 12,
    MAL_ADC_CHANNEL_13 = 13,
    MAL_ADC_CHANNEL_14 = 14,
    MAL_ADC_CHANNEL_15 = 15,
    MAL_ADC_CHANNEL_SIZE = 16
} mal_adc_channel_e;

/**
 * The initialization parameters of an ADC.
 */
typedef struct {
	mal_adc_e adc; /**< To ADC to initialize.*/
	uint8_t bit_resolution; /**< The resolution of the ADC.*/
} mal_adc_init_s;

typedef struct {
    mal_adc_channel_e channel; /**< The ADC channel to initialize.*/
    mal_gpio_port_e port; /**< The port of the ADC channel to initialize.*/
    uint8_t pin; /**< The pin of the port of the ADC channel to initialize. */
} mal_adc_channel_init_s;

/**
 * @brief This callback will be executed when an ADC read is complete.
 * @param handle The handle given for the callback.
 * @param channel The channel executing the callback.
 * @param value The value read.
 */
typedef void (*mal_adc_read_callback_t)(void *handle, mal_adc_channel_s *channel, uint64_t value);

/**
 * @brief Initialize an ADC.
 * @param init The initialization parameters.
 * @param adc The handle to initialize. This handle is used to access
 * subsequent functions.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_init(mal_adc_init_s *init, mal_adc_s *adc);

/**
 * @brief Initialise a channel of a ADC.
 * @param init The initialization parameters.
 * @param adc The initialised ADC to initialise a channel from.
 * @param channel The channel handle to initialise. This handle is used to access subsequent functions.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_channel_init(mal_adc_channel_init_s *init, mal_adc_s *adc, mal_adc_channel_s *channel);

/**
 * @brief Read an ADC channel.
 * @param channel The ADC channel to read from.
 * @param value A pointer of type uint64_t that that will contain the data
 * read.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_read_bits(mal_adc_channel_s *channel, uint64_t *value);

/**
 * @brief Get the resolution of the ADC.
 * @param adc The ADC to get the resolution from.
 * @param resolution A pointer of type uint8_t that will contain the
 * resolution.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_resolution(mal_adc_s *adc, uint8_t *resolution);

/**
 * @brief Start an async read. A callback will be executed once the read is complete. If an the ADC of that channel is
 * busy for another channel, requested will be queued. If a request for that channel is queued,
 * #MAL_ERROR_HARDWARE_UNAVAILABLE will be returned.
 * @param channel The ADC channel to read from.
 * @param callback The callback to execute once the read is complete.
 * @param callback_handle This handle will be passed to the callback function.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_async_read(mal_adc_channel_s *channel, mal_adc_read_callback_t callback, void *callback_handle);

/**
 * @brief Disable interrupts for an ADC.
 * @param handle The ADC to disable the interrupt.
 * @param state The state to use to restore interrupts.
 */
MAL_DEFS_INLINE void mal_adc_disable_interrupt(mal_adc_s *handle, mal_adc_interrupt_state_s *state);

/**
 * @brief Enable interrupts for an ADC.
 * @param handle The ADC to enable the interrupt.
 * @param state A boolean that indicates if the interrupt should be activated.
 * Use the returned state of the disable function.
 * @return Nothing. This macro is meant to be standalone on a line. Do not
 * equate or use as a condition.
 */
MAL_DEFS_INLINE void mal_adc_restore_interrupt(mal_adc_s *handle, mal_adc_interrupt_state_s *state);

/**
 * @brief Get the maximum resolution of the ADC.
 * @param adc The ADC to get the resolution from.
 * @param resolution A pointer of type uint8_t that will contain the
 * resolution.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_maximum_resolution(mal_adc_e adc, uint8_t *resolution);

/**
 * @brief Read an ADC value, but as volts instead of LSBs.
 * @param adc The ADC to which the channel is associated.
 * @param channel The ADC channel to read from.
 * @param value The value read.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_read_volts(mal_adc_s *adc, mal_adc_channel_s *channel, mal_volts_t *value);

/**
 * @brief Converts a bit value of an ADC to volts.
 * @param adc The ADC from which the bit value was read from.
 * @param bit_value The value read rom the ADC.
 * @param value The converted value.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_adc_bits_to_volts(mal_adc_s *adc, uint64_t bit_value, mal_volts_t *value);

/**
 * This include is last because it defines hardware specific implementations of
 * structures. If not included last, circular dependencies will arise.
 */
#include "hspec/mal_hspec.h"

/**
 * @}
 */

#endif /* ADC_MAL_ADC_H_ */
