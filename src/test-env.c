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

#include <stdio.h>
#include <stdlib.h>
#include "environment.h"
#include "expression.h"
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
    idStrings = (char ***)SAFE_MALLOC(sizeof(char **) * 3);
    symNames = (char ***)SAFE_MALLOC(sizeof(char **) * 3);
    symNamesLvl = (char ***)SAFE_MALLOC(sizeof(char **) * 3);
    for (level = 0; level < 3; level++) {
        idStrings[level] = (char **)SAFE_MALLOC(sizeof(char *) * no);
        symNames[level] = (char **)SAFE_MALLOC(sizeof(char *) * no);
        symNamesLvl[level] = (char **)SAFE_MALLOC(sizeof(char *) * no);
        for (i = 0; i < no; i++) {
            idStrings[level][i] = SAFE_STRING_NEW(ID_STRING_LENGTH);
            SAFE_SPRINTF(idStrings[level][i], ID_STRING_LENGTH, ID_STRING,
                         level, i);
            symNames[level][i] = SAFE_STRING_NEW(SYM_NAME_LENGTH);
            SAFE_SPRINTF(symNames[level][i], SYM_NAME_LENGTH, SYM_NAME, i);
            symNamesLvl[level][i] = SAFE_STRING_NEW(SYM_NAME_LVL_LENGTH);
            SAFE_SPRINTF(symNamesLvl[level][i], SYM_NAME_LVL_LENGTH,
                         SYM_NAME_LVL, level, i);
        }
    }
}

void deleteStrings(int no) {
    int i, level = 0;
    for (level = 0; level < 3; level++) {
        for (i = 0; i < no; i++) {
            free(idStrings[level][i]);
            free(symNames[level][i]);
            free(symNamesLvl[level][i]);
        }
        free(idStrings[level]);
        free(symNames[level]);
        free(symNamesLvl[level]);
    }
    free(idStrings);
    free(symNames);
    free(symNamesLvl);
}

void printStrings(void) {
    int level, i;
    createStrings(15);
    for (level = 0; level < 3; level++) {
        printf("Level %i\n", level);
        for (i = 0; i < 15; i++) {
            printf("%s       %s        %s\n", idStrings[level][i],
                   symNames[level][i], symNamesLvl[level][i]);
        }
    }
}

int fillEnvironment(struct Expression *env, int level, int no) {
    int i;
    char *str;
    struct Expression *expr;
    for (i = 0; i < no; i++) {
        str = symNamesLvl[level][i];
        expr = CREATE_STRING_EXPRESSION(env, str);
        ENVIRONMENT_ADD_STRING(env, symNamesLvl[level][i], expr);
        ENVIRONMENT_ADD_STRING(env, symNames[level][i], expr);
        expressionDispose(env, expr);
    }
    return 0;
}

int fillEnvironment0(void) { return fillEnvironment(env, 0, NO_ENTRIES); }

int fillEnvironment1(void) { return fillEnvironment(env1, 1, NO_ENTRIES); }

int fillEnvironment2(void) { return fillEnvironment(env2, 2, NO_ENTRIES); }

int lookup(struct Expression *env, char *str, char *reference) {
    struct Expression *res;
    struct Expression *expr;
    expr = CREATE_STRING_EXPRESSION(env, str);
    res = ENVIRONMENT_SYMBOL_LOOKUP(env, expr);
    expressionDispose(env, expr);
    if (EXPR_IS_NIL(res)) return 1;
    if (!strcmp(EXPRESSION_STRING(res), reference)) {
        expressionDispose(env, res);
        return 1;
    } else {
        expressionDispose(env, res);
        return 0;
    }
}

int testLookup(void) {
    int i;
    for (i = NO_ENTRIES - 1; i >= 0; i--) {
        /* Look up all 3 symbols */
        if (!lookup(env2, symNamesLvl[2][i], symNamesLvl[2][i])) return 1;
        if (!lookup(env2, symNamesLvl[1][i], symNamesLvl[1][i])) return 1;
        if (!lookup(env2, symNamesLvl[0][i], symNamesLvl[0][i])) return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    int result;
    struct Memory *mem = newMemory();

    DECLARE_TEST(environment.c);

    createStrings(NO_ENTRIES);
    env = environmentCreate(0, mem);
    result = test(fillEnvironment0(), "not stacked environment");
    env1 = environmentCreate(env, mem);
    expressionDispose(env, env);
    result += test(fillEnvironment1(), "stacked environment (level 2)");
    env2 = environmentCreate(env1, mem);
    expressionDispose(env, env1);
    result += test(fillEnvironment2(), "stacked environment (level 3)");
    test(testLookup(), "look ups in stacked environment");
    expressionDispose(env, env2);
    deleteMemory(mem);
    deleteStrings(NO_ENTRIES);
    return result;
}
