/*
 * fControl.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "blockio.h"
#include "fControl.h"
#include "fileSystem.h"
#include "pathUtils.h"

/*
 * createFCB: Creates a file control block.
 *
 * @parentFCB   Integer         the parent file control block
 * @name        String          name of the file control block
 *
 * return  0:                   successful execution
 * return -1:                   error in creating root directory
 */
int createFCB(int parentFCB, char* name) {
    if (parentFCB == ROOT_BLOCK && name == "") {
        if (!createRoot()) {
            fprintf(stderr, "Error in creating root directory");
            return -1;
        }
    }

    return 0;
}

/*
 * createRoot: Creates the root directory.
 *
 * return 0:    successful execution
 */
int createRoot() {
    return 0;
}

/*
 * createFile: Creates a file.
 *
 * @parentFCB   Integer         the parent file control block
 * @name        String          name of the file
 *
 * return 0:                    successful execution
 */
int createFile(int parentFCB, char* name) {
    return 0;
}

/*
 * getType: Get file type of a file from the file control block
 *
 * @type        Integer Pointer     file type
 * @fcBlock     Integer             location of the file control block
 *
 * return 0:                        successful execution
 */
int getType(int* type, int fcBlock, char* name) {
    return 0;
}

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
 * getSize: Get file size of a file from the file control block.
 *
 * @size      Integer Pointer       file size
 * @fcBlock   Integer               location of the file control block
 *
 * return 0:                        successful execution
 */
int getSize(int* size, int fcBlock, char* name) {
    return 0;
}
