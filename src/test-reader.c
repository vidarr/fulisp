/*
 * (C) 2012 Michael J. Beer 
 * This program is free software; you can
 * redistribute it and/or modify * it under the terms of the GNU General Public
 * License as published by * the Free Software Foundation; either version 3 of
 * the License, or * (at your option) any later version.2010 *2010 * This
 * program is distributed in the hope that it will be useful,2010 * but WITHOUT
 * ANY WARRANTY; without even the implied warranty of * MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the * GNU General Public License for
 * more details.2010 2010 * You should have received a copy of the GNU General
 * Public License * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.2010
 */

#include "config.h"
#include "fulispreader.h"
#include "testfileinput.h"


#define INPUT_FILE     "../tests/test-reader.in"
#define INPUT_REF_FILE "../tests/test-reader.ref"


struct Expression *getNextExpression(struct Reader *reader) {
    struct Expression *expr = fuRead(reader);
    return expr;
}


int main(int argc, char **argv) {
    int result;

    struct Expression *env;
    printf("Testing reader.c\n");
    env = environmentCreateStdEnv();
    result = checkFromFiles(env, INPUT_FILE, INPUT_REF_FILE, getNextExpression);
    expressionDispose(env, env);
    return result;
}

