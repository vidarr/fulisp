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
  
#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include "config.h"
#include "safety.h"
#include "expression.h"
#include "hash.h"
#include "stdlib.h"
#include "environment.h"



/**
 * Creates a new clean symbol table
 */
struct HashTable *symbolTableCreate(struct Expression *env);


/**
 * Disposes the symbol table and the underlying structure
 * Gets rid of every symbol contained within the table
 * @param hash the symbol table to dispose
 */
void symbolTableDispose(struct Expression *env, struct HashTable *hash); 


/**
 * Resolves the name of a symbol to the symbol itself
 * if not existent, the symbol is created
 * @param name the name of the symbol
 * @return the expression repr. the symbol
 */
struct Expression *symbolTableGetSymbol(struct Expression *env, struct HashTable *hash, char *name);


#endif
