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

#include "std/mal_stdint.h"
#include "std/mal_types.h"
#include "gpio/mal_gpio_definitions.h"
#include "std/mal_error.h"

/**
 * @defgroup DAC
 * @brief @copybrief mal_dac.h
 * @{
 */

/**
 * The possible DACs.
 */
typedef enum {
    MAL_DAC_0 = 0,   //!< DAC interface 0
    MAL_DAC_1 = 1,   //!< DAC interface 1
    MAL_DAC_2 = 2,   //!< DAC interface 2
    MAL_DAC_SIZE = 3 //!< Number of possible DAC interfaces
} mal_dac_e;

/**
 * DAC handle that must be defined by hardware specific implementation. Used
 * to access the dac functions.
 */
typedef struct MAL_DAC mal_dac_s;

/**
 * The initialization parameters of a DAC.
 */
typedef struct {
    mal_dac_e dac; /**< To DAC to initialize.*/
    mal_gpio_port_e port; /**< The port of the GPIO of the DAC.*/
    uint8_t pin; /**< The pin of the GPIO of the DAC.*/
    uint8_t bit_resolution; /**< The resolution of the DAC.*/
} mal_dac_init_s;

/**
 * @brief Write to a DAC.
 * @param handle The DAC to write to.
 * @param value A value of type uint64_t that that contains the data to write.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_dac_write_bits(mal_dac_s *handle, uint64_t value);

/**
 * @brief Get the resolution of the DAC.
 * @param handle The DAC to get the resolution from.
 * @param resolution A pointer of type uint8_t that will contain the
 * resolution.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_dac_resolution(mal_dac_s *handle, uint8_t *resolution);

/**
 * @brief Get the maximum resolution of the DAC.
 * @param dac The DAC to get the resolution from.
 * @param resolution A pointer of type uint8_t that will contain the
 * resolution.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_dac_maximum_resolution(mal_dac_e dac, uint8_t *resolution);

/**
 * @brief Initialize a DAC.
 * @param init The initialization parameters.
 * @param handle The handle to initialize. Use this handle to access subsequent
 * DAC functions.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_dac_init(mal_dac_init_s *init, mal_dac_s *handle);

/**
 * @brief Write to a DAC, but as volts instead of LSBs.
 * @param handle The DAC to write to.
 * @param value The value to write.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_dac_write_volts(mal_dac_s *handle, mal_volts_t value);

/**
 * @brief Convert a bit value of a DAC to volts.
 * @param handle The DAC to which the bit value will be written to.
 * @param value The value to write.
 * @param bit_value The converted value.
 * @return Returns #MAL_ERROR_OK on success.
 */
mal_error_e mal_dac_volts_to_bits(mal_dac_s *handle, mal_volts_t value, uint64_t *bit_value);

/**
 * This include is last because it defines hardware specific implementations of
 * structures. If not included last, circular dependencies will arise.
 */
#include "hspec/mal_hspec.h"

/**
 * @}
 */

#endif /* DAC_MAL_DAC_H_ */
