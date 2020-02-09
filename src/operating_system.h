/*
 * (C) 2020 Michael J. Beer
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
#ifndef OPERATING_SYSTEM_H
#define OPERATING_SYSTEM_H

#include <stdio.h>

/*----------------------------------------------------------------------------*/

/**
 * Tries to open a file with name `fname` in `mode`.
 * If path is not 0, the file that will be opened is 'path/fname'
 * See `fopen(3)` for details on mode.
 */
FILE * file_open(char const *path, char const *fname, char const *mode);

/*----------------------------------------------------------------------------*/
#endif

