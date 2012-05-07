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
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <limits.h>

#define DEBUG
#define DEBUG_READER 
#define DEBUG_EVAL
/* #define DEBUG_FUNCTIONS */
#define DEBUG_EXPRESSION
#define DEBUG_SYMBOL_TABLE
#define DEBUG_STACK

#define HASH_TABLE_SAFE_KEYS
#define GENERATE_SAFETY_CODE

#define READ_BUFFER_SIZE (int)(16000)
#define PRINT_BUFFER_SIZE 100
#define SYMBOL_TABLE_SIZE 20
#define CALL_STACK_SIZE 200


#endif

