/*
 * (C) 2012 Michael J. Beer
 * All rights reserved.
 *
 * Redistribution  and use in source and binary forms, with or with‐
 * out modification, are permitted provided that the following  con‐
 * ditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above  copy‐
 * right  notice,  this  list  of  conditions and the following dis‐
 * claimer in the documentation and/or other materials provided with
 * the distribution.
 *
 * 3.  Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote  products  derived
 * from this software without specific prior written permission.
 *
 * THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBU‐
 * TORS "AS IS" AND ANY EXPRESS OR  IMPLIED  WARRANTIES,  INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT
 * SHALL  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DI‐
 * RECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS IN‐
 * TERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY  THEORY  OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  NEGLI‐
 * GENCE  OR  OTHERWISE)  ARISING  IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PRINT_NATIVE_FUNCTIONS_H__
#define __PRINT_NATIVE_FUNCTIONS_H__

/* Convert decimal no between 0 and 15 to hex digit */
#define TO_HEX(x) ((x) < 10 ? '0' + (x) : 'A' + (x)-10)

/* Convert x to hex, get low hex digit */
#define HEX_LOW_DIGIT(x) TO_HEX(((x) & (16 | 8 | 4 | 2 | 1)))

/* Convert x to hex, get high hex digit */
#define HEX_HIGH_DIGIT(x) TO_HEX(((x) >> 4))

/**
 * This macro explicitly prints out function pointers in a compatible way - so
 * it does not respect the STRICT_NATIVE_FUNC switch
 * @param func the function pointer to print
 * @param str the string to write result to. must provide 3 * sizeof(function
 * pointer)
 */
#define NATIVE_FUNC_TO_STR(func, str, bufLen)             \
    {                                                     \
        int i;                                            \
        unsigned char *fp;                                \
        size_t bufLenUsed = 3 * sizeof(NativeFunction *); \
        IF_SAFETY_CODE(assert(bufLen >= bufLenUsed););    \
        fp = (unsigned char *)&func;                      \
        for (i = 0; i < sizeof(NativeFunction *); i++) {  \
            str[3 * i] = ' ';                             \
            str[3 * i + 1] = HEX_HIGH_DIGIT(fp[i]);       \
            str[3 * i + 2] = HEX_LOW_DIGIT(fp[i]);        \
        };                                                \
        str[bufLenUsed] = '\0';                           \
    };

#endif
