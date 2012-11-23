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
#include "openFiles.h"
#include "pathUtils.h"
#include "storeInt.h"
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

    char* next = encode_int(END_BLOCK);

    block[NEXT_BLOCK] = next[0];
    block[NEXT_BLOCK + 1] = next[1];

    if (put_block(startingBlockID, block)) {
        fprintf(stderr, "Error creating file block.\n");
        return -2;
    }

    return 0;
}

/*
 * deleteDir: Deletes a directory
 *
 * precondition: length of name is of valid length
 *
 * @fcBlockID   Integer     the parent file control block
 * @name        String      name of the directory
 *
 * return  0:               successful execution
 * return -1:               error finding the position of the file block
 * return -2:               error retrieving the file block
 * return -3:               directory is not empty
 * return -4:               error creating file block
 * return -5:               error removing entry from the file control block
 */
int deleteDir(int fcBlockID, char* name) {
    int blockID;

    if (getStart(&blockID, fcBlockID, name)) {
        fprintf(stderr, "Error finding the position of the file block.\n");
        return -1;
    }

    char* block = malloc(BLOCK_SIZE);

    if (get_block(blockID, block)) {
        fprintf(stderr, "Error retrieving the file block.\n");
        return -2;
    }

    for (int i = ENTRY_START; i < BLOCK_SIZE; i++) {
        if (block[i] != ENTRY_END && block[i] != '\0') {
            fprintf(stderr, "Directory is not empty.\n");
            return -3;
        }
    }

    block[BLOCK_START] = FREE;

    if (put_block(blockID, block)) {
        fprintf(stderr, "Error creating file block.\n");
        return -4;
    }

    if (fcBlockID != ROOT_BLOCKID || strcmp(name, ROOT)) {
        if (removeEntry(&blockID, fcBlockID, name)) {
            fprintf(stderr, "Error removing entry from the file control block.\n");
            return -5;
        }
    }

    // Remove all entries referencing the deleted file from the file open table
    deleteAll(blockID);

    return 0;
}

/*
 * deleteFile: Deletes a file
 *
 * precondition: length of name is of valid length
 *
 * @fcBlockID   Integer     the parent file control block
 * @name        String      name of the directory
 *
 * return  0:               successful execution
 * return -1:               error removing entry from the file control block
 * return -2:               error retrieving file block
 * return -3:               error creating file block
 */
int deleteFile(int fcBlockID, char* name) {
    int currentBlock;

    // Remove references to the file from the parent file control block
    if (removeEntry(&currentBlock, fcBlockID, name)) {
        fprintf(stderr, "Error removing entry from the file control block.\n");
        return -1;
    }

    // Remove all entries referencing the deleted file from the file open table
    deleteAll(currentBlock);

    do {
        char* block = malloc(BLOCK_SIZE);

        if (get_block(currentBlock, block)) {
            fprintf(stderr, "Error retrieving file block.\n");
            return -2;
        }

        // Label the block as free
        block[BLOCK_START] = FREE;

        if (put_block(currentBlock, block)) {
            fprintf(stderr, "Error creating file block.\n");
            return -3;
        }

        char next[2];
        next[0] = block[NEXT_BLOCK];
        next[1] = block[NEXT_BLOCK + 1];

        // Get the next referenced block
        currentBlock = decode_int(next);
    } while (currentBlock != END_BLOCK);

    return 0;
}
