/*
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
    
#ifndef __SYS_H__
#define __SYS_H__

/******************************************************************************
                       Operating System dependent things
 ******************************************************************************/


#ifdef __OpenBSD__  /* Operating System */

#    define CLOCK_PARAMETER CLOCK_VIRTUAL
#    include <sys/time.h>

#else 

    /* We assume a GNU system in here - not the best thing to do, but a com
       promise between effort and practicability */

#    define CLOCK_PARAMETER CLOCK_PROCESS_CPUTIME_ID
#    include <time.h>

#endif /* Operating System */


#endif

