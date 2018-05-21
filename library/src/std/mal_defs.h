/**
 * @file mal_types.h
 * @author Olivier Allaire
 * @date July 05 2017
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
 * @brief Definitions of key elements for different platforms.
 */

#ifndef STD_MAL_DEFS_H_
#define STD_MAL_DEFS_H_

/**
 * @defgroup DEFINITIONS
 * @brief @copybrief mal_defs.h
 * @{
 */

#ifdef MAL_GNU

/**
 * Skip inlines for MINGW.
 */
#define MAL_DEFS_INLINE

#else

#define MAL_DEFS_INLINE     __attribute__ ((gnu_inline)) inline

#endif

#define MAL_DEFS_NO_RETURN  __attribute__ ((section(".after_vectors"),noreturn))

/**
 * Use to remove compiler/IDE warnings.
 */
#define MAL_DEFS_UNUSED(value) (void)(value)

/**
 * @}
 */

#endif /* STD_MAL_DEFS_H_ */
