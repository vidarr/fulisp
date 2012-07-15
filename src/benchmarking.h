/**
 * (C) 2012 Michael J. Beer
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
    
#ifndef __BENCHMARK_H__
#define __BENCHMARK_H__



#ifdef BENCHMARK


#include <time.h>
#include <limits.h>
#include <stdlib.h>


long benchmarkTest(void);

long benchmarkGetTime(void);


#define BENCHMARK_DECLARE_VAR(x, y, temp)  long x, y, temp;


#define BENCHMARK_INIT(x, y, temp)  { \
    x = 0; y = 0; \
    if((temp = benchmarkTest()) < 0) { \
        fprintf(stderr, "Warning: Could not get resolution of clock." \
                        " Benchmarking wont be possible\n"); \
    } else { \
        fprintf(stderr, "Benchmarking: Resolution of clock will be %li\n", \
                temp); \
    }; \
}


#define BENCHMARK_CONTINUE(x, y, temp) { \
    if((temp = benchmarkGetTime()) < 0) { \
        fprintf(stderr, "Benchmarking: Could not read clock - disabling" \
                        " benchmark.\n"); \
        x = LONG_MIN; \
    } else { \
        y = temp; \
   }; \
}


#define BENCHMARK_INTERRUPT(x, y, temp) { \
    if((temp = benchmarkGetTime()) < 0 || x == LONG_MIN) { \
        fprintf(stderr, "Benchmarking: Could not read clock - disabling" \
                        " bechmark.\n"); \
        x = LONG_MIN;  \
    } else { \
        x += labs(y - temp); \
        fprintf(stderr, "Benchmarking: Measured %li\n", labs(y - temp)); \
    }; \
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

