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

#include <stdlib.h>
#include <stdio.h>
#include "expression.h"
#include "environment.h"
#include "test.h"


#define ID_STRING "Level %i: SYM %i"
#define ID_STRING_LENGTH 40

#define SYM_NAME "SYM%i"
#define SYM_NAME_LENGTH 10

#define SYM_NAME_LVL "L%iSYM%i"
#define SYM_NAME_LVL_LENGTH 15

#define NO_ENTRIES 10


char ***idStrings;
char ***symNames, ***symNamesLvl;

struct Expression *env1; 
struct Expression *env2; 
struct Expression *env; 


void createStrings(int no) {
    int i, level;
    idStrings = (char ***)malloc(sizeof(char **) * 3);
    symNames = (char ***)malloc(sizeof(char **) * 3);
    symNamesLvl = (char ***)malloc(sizeof(char **) * 3);
    for(level = 0; level < 3; level++) {
        idStrings[level] = (char **)malloc(sizeof(char *) * no);
        symNames[level] = (char **)malloc(sizeof(char *) * no);
        symNamesLvl[level] = (char **)malloc(sizeof(char *) * no);
        for(i = 0; i < no; i++) {
            idStrings[level][i] = (char *)malloc(sizeof(char) * ID_STRING_LENGTH);
            sprintf(idStrings[level][i], ID_STRING, level, i);
            symNames[level][i] = (char *)malloc(sizeof(char)
                    * SYM_NAME_LENGTH);
            sprintf(symNames[level][i], SYM_NAME, i);
            symNamesLvl[level][i] = (char *)malloc(sizeof(char)
                    * SYM_NAME_LVL_LENGTH);
            sprintf(symNamesLvl[level][i], SYM_NAME_LVL, level, i);
        }
    }
}


void printStrings(void) { 
    int level, i;
    createStrings(15);
    for(level = 0; level < 3; level++) {
        printf("Level %i\n", level);
        for(i = 0; i < 15; i++) {
            printf("%s       %s        %s\n", idStrings[level][i],
                    symNames[level][i], symNamesLvl[level][i]);
        }
    }
}



int fillEnvironment(struct Expression *env, int level, int no) { 
 
    int i;
    struct Expression *expr;
    for(i = 0; i < no; i++) {
        expr = expressionCreate(env, EXPR_STRING, symNamesLvl[level][i]);
        ENVIRONMENT_ADD_STRING(env, symNamesLvl[level][i], expr); 
        ENVIRONMENT_ADD_STRING(env, symNames[level][i], expr); 
        expressionDispose(env, expr);
    }
    return 0;
}



int fillEnvironment0(void) {
    return fillEnvironment(env, 0,  NO_ENTRIES);
}
 
 
int fillEnvironment1(void) {
    return fillEnvironment(env1, 1,  NO_ENTRIES);
}
 

int fillEnvironment2(void) {
    return fillEnvironment(env2, 2,  NO_ENTRIES);
}


int lookup(struct Expression *env, char *str, char *reference) {
    struct Expression *res;
    struct Expression *expr = expressionCreate(env, EXPR_STRING, str);
    res = ENVIRONMENT_SYMBOL_LOOKUP(env, expr);
    expressionDispose(env, expr);
    if(!res) return 1;
    if(!strcmp(EXPRESSION_STRING(res), reference)) {
        return 1;
    } else {
        return 0;
    }
}


int testLookup(void) {
    int i;
    for(i = NO_ENTRIES - 1; i >= 0; i--) {
        /* Look up all 3 symbols */
        if(!lookup(env2, symNames[2][i], symNamesLvl[2][i]))
            return 1;
        if(!lookup(env2, symNamesLvl[1][i], symNamesLvl[1][i]))
            return 1;
        if(!lookup(env2, symNamesLvl[0][i], symNamesLvl[0][i]))
            return 1;
    }
    return 0;
}


int main(int argc, char **argv) { 
    int result;
    createStrings(NO_ENTRIES);
    env = environmentCreate(0); 
    result = test(fillEnvironment0(), "not stacked environment"); 
    env1 = environmentCreate(env); 
    expressionDispose(env, env); 
    result += test(fillEnvironment1(), "stacked environment (level 2)"); 
    env2 = environmentCreate(env1); 
    expressionDispose(env, env1); 
    result += test(fillEnvironment2(), "stacked environment (level 3)"); 
    test(testLookup(), "look ups in stacked environment"); 
    expressionDispose(env, env2); 
    return result;
} 

