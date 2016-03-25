/*
 * mal_hspec_mingw_uuid.h
 *
 *  Created on: Mar 25, 2016
 *      Author: Olivier
 */

#ifndef HSPEC_MINGW_MAL_HSPEC_MINGW_UUID_H_
#define HSPEC_MINGW_MAL_HSPEC_MINGW_UUID_H_

#include "std/mal_error.h"
#include "std/mal_stdint.h"

mal_error_e mal_hspec_mingw_uuid_read(uint64_t *uuid);

#endif /* HSPEC_MINGW_MAL_HSPEC_MINGW_UUID_H_ */
