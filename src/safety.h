/**
 * (C) 2011 Michael J. Beer
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
 *
 * Safe code will only be generated if GENERATE_SAFETY_CODE has been set
 * Else, the following defs will have *no* effekt
 */
  
#ifndef __SAFETY_H__
#define __SAFETY_H__

#include "config.h"


/**
 * This determines the number of safety checks that will be performed
 */
extern int safetyLevel;


/*****************************************************************************
 *        Security levels
 *****************************************************************************/

/**
 * Perform all safety tests
 */
#define SAFETY_HIGH 255

/**
 * Perform no safety checks
 */
#define SAFETY_LOW    0


#ifdef GENERATE_SAFETY_CODE

/******************************************************************************
 *      Macros to execute code if some safety level has been set
 *****************************************************************************/

/**
 * Execute code only if safety level is set to SAFETY_HIGH
 */
#define IF_SAFETY_HIGH(x) { if(safetyLevel == SAFETY_HIGH) {x}}

/** 
 * Compiles the code given if GENERATE_SAFE_CODE is set
 */
#define IF_SAFETY_CODE(x) {x}


/******************************************************************************
 *                           Manipulate safety level
 *****************************************************************************/

/**
 * Set safety level
 */
#define SET_SAFETY_LEVEL(x) {safetyLevel = x;}

/**
 * Set safety level to SAFETY_LOW
 */
#define SET_SAFETY_LOW SET_SAFETY_LEVEL(SAFETY_LOW)


/**
 * Set safety level to SAFETY_HIGH
 */
#define SET_SAFETY_HIGH SET_SAFETY_LEVEL(SAFETY_HIGH)



#else
/* if no safe code should be generated, just remove all safety macros */

#define IF_SAFETY_HIGH(x)

#define IF_SAFETY_CODE(x)

#define SET_SAFETY_LEVEL(x)

#define SET_SAFETY_LOW 

#define SET_SAFETY_HIGH 

#endif


#endif
