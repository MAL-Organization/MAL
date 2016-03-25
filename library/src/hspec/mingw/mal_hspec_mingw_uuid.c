/*
 * mal_hspec_mingw_uuid.c
 *
 *  Created on: Mar 25, 2016
 *      Author: Olivier
 */

#include <stdlib.h>
#include <time.h>

#include "mal_hspec_mingw_uuid.h"

mal_error_e mal_hspec_mingw_uuid_read(uint64_t *uuid) {
	srand(time(NULL));
	*uuid = (uint64_t)rand();
	return MAL_ERROR_OK;
}
