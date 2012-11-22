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
     *   0           -128           0
     * 127             -1           0
     * 128              0           0
     * 129              1           0
     * 254            126           0
     * 255            127           0
     * 382            254           0
     * 383            255           0
     * 384           -128           1
     * 385           -127           1
     * 509             -2           1
     * 510             -1           1
     * 511              0           1
     */

    if (c < 384) {
        output[0] = c - 128;
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
     * int      output[0]   output[1]
     *   0           -128           0
     * 127             -1           0
     * 128              0           0
     * 129              1           0
     * 254            126           0
     * 255            127           0
     * 382            254           0
     * 383            255           0
     * 384           -128           1
     * 385           -127           1
     * 509             -2           1
     * 510             -1           1
     * 511              0           1
     */

    if (c[1] == 0) {
        return c[0] + 128;
    }

    return c[0] + 511;
}
