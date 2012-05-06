/**
 * (C) 2010 Michael J. Beer
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */
  
#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <math.h>
#include "limits.h"
#include "float.h"


/* Helpers */
#define LOG_TO_LOG10_FACTOR  (1.0/log(10.0))

/**
 * Number of chars maximal used for string rep. of some data types
 */

#define MAX_BYTES_PER_int ((LOG_TO_LOG10_FACTOR * log(INT_MAX)) + 1)
#define MAX_BYTES_PER_float   ((LOG_TO_LOG10_FACTOR * log(FLT_MAX) * 2) + 1)
#define MAX_BYTES_PER_char    1
#define MAX_BYTES_PER_pointer (sizeof(void *) * pow(2,8) / 16.0)

#endif
