/*
 * fControl.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "blockio.h"
#include "fControl.h"

/*
 * getStart: Get the starting block of the file path component from the file control block.
 *
 * @block       Integer Pointer     starting block id of the file component
 * @fcBlock     Integer             the file control block id
 * @name        String              the file component
 *
 * return 0:                        successful execution
 * return 1:                        error retrieving the file control block
 */
int getStart(int* blockID, int fcBlockID, char* name) {
    char* fcBlock = malloc(BLOCK_SIZE);

    if (!get_block(fcBlockID, fcBlock)) {
        fprintf(stderr, "Error retrieving the file control block.\n");
        return 1;
    }

    return 0;
}

/*
 * createFCB: Creates a file control block.
 *
 * @blockID     Integer         block id of the file control block
 * @name        String          name of the file control block
 *
 * return 0:                    successful execution
 */
int createFCB(int blockID, char* name) {
    return 0;
}
