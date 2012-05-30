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

#ifndef __PRINT_NATIVE_FUNCTIONS_H__
#define __PRINT_NATIVE_FUNCTIONS_H__


/* Convert decimal no between 0 and 15 to hex digit */
#define TO_HEX(x) ((x) < 10 ? '0' + (x) : 'A' + (x) - 10)

/* Convert x to hex, get low hex digit */
#define HEX_LOW_DIGIT(x)  TO_HEX(((x) & (16 | 8 | 4 | 2 | 1)))

/* Convert x to hex, get high hex digit */
#define HEX_HIGH_DIGIT(x) TO_HEX(((x) >> 4))


/**
 * This macro explicitly prints out function pointers in a compatible way - so
 * it does not respect the STRICT_NATIVE_FUNC switch
 * @param func the function pointer to print
 * @param str the string to write result to. must provide 3 * sizeof(function
 * pointer)
 */
#define NATIVE_FUNC_TO_STR(func, str) { \
    int i; \
    unsigned char *fp = (unsigned char *)&func; \
    for(i = 0; i < sizeof(NativeFunction *); i += 3) { \
        str[i] = ' '; \
        str[i + 1] = HEX_HIGH_DIGIT(fp[i]); \
        str[i + 2] = HEX_LOW_DIGIT(fp[i]); \
    }; \
};

#endif
