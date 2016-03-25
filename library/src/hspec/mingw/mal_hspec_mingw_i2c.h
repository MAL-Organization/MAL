/*
 * mal_hspec_mingw_i2c.h
 *
 *  Created on: Mar 24, 2016
 *      Author: Olivier
 */

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_I2C_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_I2C_H_

#include "std/mal_error.h"
#include "hspec/mal_hspec_cmn.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_mingw_get_valid_i2c_ios(mal_hspec_i2c_e interface, const mal_hspec_gpio_s **scls, uint8_t *scls_size, const mal_hspec_gpio_s **sdas, uint8_t *sdas_size);

mal_error_e mal_hspec_mingw_i2c_master_init(mal_hspec_i2c_init_s *init);

mal_error_e mal_hspec_mingw_i2c_transfer(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg);

mal_error_e mal_hspec_mingw_i2c_get_transfer_msg(mal_hspec_i2c_e interface, mal_hspec_i2c_msg_s *msg);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_I2C_H_ */
