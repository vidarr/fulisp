/*
 * (C) 2020 Michael J. Beer
 * All rights reserved.
 *
 * Redistribution  and use in source and binary forms, with or with‐
 * out modification, are permitted provided that the following  con‐
 * ditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above  copy‐
 * right  notice,  this  list  of  conditions and the following dis‐
 * claimer in the documentation and/or other materials provided with
 * the distribution.
 *
 * 3.  Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote  products  derived
 * from this software without specific prior written permission.
 *
 * THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBU‐
 * TORS "AS IS" AND ANY EXPRESS OR  IMPLIED  WARRANTIES,  INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT
 * SHALL  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DI‐
 * RECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS IN‐
 * TERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY  THEORY  OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  NEGLI‐
 * GENCE  OR  OTHERWISE)  ARISING  IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef OPERATING_SYSTEM_H
#define OPERATING_SYSTEM_H

/******************************************************************************
 *                           OS Compliance checking
 ******************************************************************************/

#if ! defined (__unix__)

#error("No POSIX compatible operating system found")

#endif

#include <unistd.h>

#ifndef _POSIX_VERSION

#error("No POSIX compatible operating system found")

#endif

/*----------------------------------------------------------------------------*/

#ifdef __linux__

#include <linux/limits.h>

#endif

#ifndef PATH_MAX

#define PATH_MAX _POSIX_PATH_MAX

#endif

/******************************************************************************
 *                                   Actual
 ******************************************************************************/

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

