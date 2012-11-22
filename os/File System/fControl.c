/*
 * fControl.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blockio.h"
#include "fControl.h"
#include "fileSystem.h"
#include "pathUtils.h"
#include "entry.h"

/*
 * createRoot: Creates the root directory.
 *
 * return  0:       successful execution
 * return -1:       error creating file block
 */
int createRoot() {
    char* block = malloc(BLOCK_SIZE);

    block[BLOCK_START] = DIRECTORY;
    block[ENTRY_START] = ENTRY_END;

    if (put_block(ROOT_BLOCKID, block)) {
        fprintf(stderr, "Error creating file block.\n");
        return -1;
    }

    return 0;
}

/*
 * createFCB: Creates a file control block.
 *
 * precondition: length of name is of valid length
 *
 * @parentFCBID     Integer         the parent file control block
 * @name            String          name of the file control block
 *
 * return  0:                       successful execution
 * return -1:                       error in creating root directory
 * return -2:                       error adding entry to file control block
 * return -3:                       error creating file block
 */
int createFCB(int parentFCBID, char* name) {
    if (parentFCBID == ROOT_BLOCKID && !strcmp(name, ROOT)) {
        if (createRoot()) {
            fprintf(stderr, "Error in creating root directory.\n");
            return -1;
        }

        return 0;
    }

    int startingBlockID;

    if (addEntry(&startingBlockID, parentFCBID, name, 1)) {
        fprintf(stderr, "Error adding entry to file control block.\n");
        return -2;
    }

    char* block = malloc(BLOCK_SIZE);

    block[BLOCK_START] = DIRECTORY;
    block[ENTRY_START] = ENTRY_END;

    if (put_block(startingBlockID, block)) {
        fprintf(stderr, "Error creating file block.\n");
        return -3;
    }

    return 0;
}

/*
 * createFile: Creates a file.
 *
 * precondition: length of name is of valid length
 *
 * @fcBlockID   Integer     the parent file control block
 * @name        String      name of the file
 *
 * return  0:               successful execution
 * return -1:               error adding entry to file control block
 * return -2:               error creating file block
 */
int createFile(int fcBlockID, char* name) {
    int startingBlockID;

    if (addEntry(&startingBlockID, fcBlockID, name, 0)) {
        fprintf(stderr, "Error adding entry to file control block.\n");
        return -1;
    }

    char* block = malloc(BLOCK_SIZE);

    block[BLOCK_START] = FILE;
    block[1] = END_BLOCK;

    if (put_block(startingBlockID, block)) {
        fprintf(stderr, "Error creating file block.\n");
        return -2;
    }

    return 0;
}
