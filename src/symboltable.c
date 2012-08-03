/*
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

#include "symboltable.h"
#include "config.h"


#define MODULE_NAME "symboltable.h"

#ifdef DEBUG_SYMBOL_TABLE
#include "debugging.h"
#else 
#include "no_debugging.h"
#endif

struct HashTable *symbolTableCreate(struct Expression *env) {
    return hashTableCreate(SYMBOL_TABLE_SIZE, stdHashFunction);
}


void symbolTableDispose(struct Expression *env, struct HashTable *hash) {
    int i;
    char *sym;
    char **symbols = hashTableKeys(hash);
    i = 0;
    while((sym = symbols[i++])) 
        expressionDispose(env, (struct Expression *)hashTableGet(hash, sym));
    free(symbols);
    hashTableDispose(hash);
}


struct Expression *symbolTableGetSymbol(struct Expression *env, struct HashTable *hash, char *name) {
    struct Expression *retVal;
    char *newName;
    assert(hash && name);

    DEBUG_PRINT_PARAM("Search fo symbol %s ...\n", name);
    if(! (retVal = (struct Expression *)hashTableGet(hash, name))) {
        DEBUG_PRINT_PARAM("Symbol %s not found\n", name);
        newName = (char *)malloc(sizeof(char) * (1 + strlen(name)));
        strcpy(newName, name);
        retVal = expressionCreate(env, EXPR_SYMBOL, newName);
        hashTableSet(hash, newName, retVal);
    }
    return expressionAssign(env, retVal); 
}

