/*
 * (C) 2012 Michael J. Beer
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

#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__

#ifdef BENCHMARK

#include <limits.h>
#include <stdlib.h>
#include <sys/time.h>

long benchmarkTest(void);

long benchmarkGetTime(void);

#define BENCHMARK_DECLARE_VAR(x, y, temp) long x, y, temp;

#define BENCHMARK_INIT(x, y, temp)                                             \
    {                                                                          \
        x = 0;                                                                 \
        y = 0;                                                                 \
        if ((temp = benchmarkTest()) < 0) {                                    \
            fprintf(stderr,                                                    \
                    "Warning: Could not get resolution of clock."              \
                    " Benchmarking wont be possible\n");                       \
        } else {                                                               \
            fprintf(stderr, "Benchmarking: Resolution of clock will be %li\n", \
                    temp);                                                     \
        };                                                                     \
    }

#define BENCHMARK_CONTINUE(x, y, temp)                               \
    {                                                                \
        if ((temp = benchmarkGetTime()) < 0) {                       \
            fprintf(stderr,                                          \
                    "Benchmarking: Could not read clock - disabling" \
                    " benchmark.\n");                                \
            x = LONG_MIN;                                            \
        } else {                                                     \
            y = temp;                                                \
        };                                                           \
    }

#define BENCHMARK_INTERRUPT(x, y, temp)                                      \
    {                                                                        \
        if ((temp = benchmarkGetTime()) < 0 || x == LONG_MIN) {              \
            fprintf(stderr,                                                  \
                    "Benchmarking: Could not read clock - disabling"         \
                    " bechmark.\n");                                         \
            x = LONG_MIN;                                                    \
        } else {                                                             \
            x += labs(y - temp);                                             \
            fprintf(stderr, "Benchmarking: Measured %li\n", labs(y - temp)); \
        };                                                                   \
    }

#define BENCHMARK_DO(x) x

#else

#define BENCHMARK_DECLARE_VAR(x, y, temp)

#define BENCHMARK_INIT(x, y, temp)

#define BENCHMARK_CONTINUE(x, y, temp)

#define BENCHMARK_INTERRUPT(x, y, temp)

#define BENCHMARK_DO(x)

#endif

#endif
