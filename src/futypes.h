/*
 * (C) 2014 Michael J. Beer
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */ 

#ifndef __FU_TYPES_H__
#define __FU_TYPES_H__



#include "expression.h"
#include "stdlib.h"

struct Expression *fuType(struct Expression *env, struct Expression *expr);

struct Expression *fuInt(struct Expression *env, struct Expression *expr);

struct Expression *fuFloat(struct Expression *env, struct Expression *expr);



#endif

