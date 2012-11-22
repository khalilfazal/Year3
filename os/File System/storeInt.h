/*
 * storeInt.h
 *
 */

#define START 2

// Stores an integer as an array of chars
char* encode_int(int c);

// Restores an integer from an array of chars
int decode_int(char* c);
