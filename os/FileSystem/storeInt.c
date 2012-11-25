/*
 * storeInt.c
 *
 */

#include <stdlib.h>
#include "storeInt.h"

/*
 * encode_int: Stores an integer as an array of chars.
 *
 * @c             Integer
 *
 * return char*     String
 */
char* encode_int(int c) {
    char* output = malloc(START);

    /*
     * int      output[0]   output[1]
     *  -1           -128           0
     *   0           -127           0
     * 127              0           0
     * 128              1           0
     * 129              2           0
     * 254            127           0
     * 255            128           0
     * 382            255           0
     *
     * 383           -128           1
     * 384           -127           1
     * 385           -126           1
     * 509             -2           1
     * 510             -1           1
     * 511              0           1
     */

    if (c < 383) {
        output[0] = c - 127;
        output[1] = 0;
    } else {
        output[0] = c - 511;
        output[1] = 1;
    }

    return output;
}

/*
 * decode_int: Restores an integer from an array of chars.
 *
 * @c             String
 *
 * return int     Integer
 */
int decode_int(char* c) {

    /*
     * int        char[0]     char[1]
     *  -1           -128           0
     *   0           -127           0
     * 127              0           0
     * 128              1           0
     * 129              2           0
     * 254            127           0
     * 255            128           0
     * 382            255           0
     *
     * 383           -128           1
     * 384           -127           1
     * 385           -126           1
     * 509             -2           1
     * 510             -1           1
     * 511              0           1
     */

    if (c[1] == 0) {
        return c[0] + 127;
    }

    return c[0] + 511;
}
