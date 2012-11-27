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

    char* next = encode_int(BLOCK_END);

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
int deleteDir(int fcBlockID, const char* name) {
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
        if (removeEntry(malloc(1), fcBlockID, name)) {
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
int deleteFile(int fcBlockID, const char* name) {
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
    } while (currentBlock != BLOCK_END);

    return 0;
}

/*
 * writeFile: writes to a file
 *
 * @blockID         Integer     location of the starting block of a file
 * @start           Integer     position in the file
 * @length          Integer     length of mem_pointer
 * @mem_pointer     String      null terminated, contains no white-spaces
 *
 * return  0:                   successful execution
 * return -1:                   invalid start value
 * return -2:                   invalid length
 * return -3:                   can't find a free block
 * return -4:                   error retrieving file block
 * return -5:                   error creating file block
 */
int writeFile(const char* mem_pointer, int blockID, int start, unsigned int length) {
    if (start < 0) {
        fprintf(stderr, "Invalid start value.\n");
        return -1;
    }

    if (length != strlen(mem_pointer)) {
        fprintf(stderr, "Invalid length.\n");
        return -2;
    }

    /*
     * while start is > DATA_SIZE:
     *      - get a free block -> nextBlockID
     *      - get_block with blockID
     *      - write encode_int(nextBlockID) to NEXT_BLOCK and NEXT_BLOCK + 1
     *      - put_block with blockID
     *      - blockID = nextBlockID
     *      - start -= DATA_SIZE
     */

    char* block;
    int nextBlockID;
    char* next;

    while (start > DATA_SIZE) {
        if (getFreeBlock(&nextBlockID)) {
            fprintf(stderr, "Can't find a free block.\n");
            return -3;
        }

        block = malloc(BLOCK_SIZE);

        if (get_block(blockID, block)) {
            fprintf(stderr, "Error retrieving file block.\n");
            return -4;
        }

        next = encode_int(nextBlockID);
        block[NEXT_BLOCK] = next[0];
        block[NEXT_BLOCK + 1] = next[1];

        if (put_block(blockID, block)) {
            fprintf(stderr, "Error creating file block.\n");
            return -5;
        }

        blockID = nextBlockID;
        start -= DATA_SIZE;
    }

    block = malloc(BLOCK_SIZE);

    if (get_block(blockID, block)) {
        fprintf(stderr, "Error retrieving file block.\n");
        return -4;
    }

    // Next character in mem_pointer to write
    int c = 0;

    // Current position in the file block
    int p = DATA_P + start;

    /*
     * while length > 0
     *      block[BLOCK_START] = FILE
     *
     *      while p < BLOCK_SIZE and length > 0
     *          block[p++] = mem_pointer[c++]
     *          length--
     *
     *      if length > 0
     *          - get a free block -> nextBlockID
     *          - write encode_int(nextBlockID) to NEXT_BLOCK and NEXT_BLOCK + 1
     *          - put_block with blockID
     *          - blockID = nextBlockID
     *          - block = malloc(BLOCK_SIZE)
     *          - get_block with blockID
     *          - p = DATA_P
     *
     * write encode_int(BLOCK_END) to NEXT_BLOCK and NEXT_BLOCK + 1
     * put_block with blockID
     * return 0
     */

    while (length > 0) {
        block[BLOCK_START] = FILE;

        while (p < BLOCK_SIZE && length > 0) {
            block[p++] = mem_pointer[c++];
            length--;
        }

        if (length > 0) {
            if (getFreeBlock(&nextBlockID)) {
                fprintf(stderr, "Can't find a free block.\n");
                return -3;
            }

            next = encode_int(nextBlockID);
            block[NEXT_BLOCK] = next[0];
            block[NEXT_BLOCK + 1] = next[1];

            if (put_block(blockID, block)) {
                fprintf(stderr, "Error creating file block.\n");
                return -5;
            }

            blockID = nextBlockID;
            block = malloc(BLOCK_SIZE);

            if (get_block(blockID, block)) {
                fprintf(stderr, "Error retrieving file block.\n");
                return -4;
            }

            p = DATA_P;

        }
    }

    next = encode_int(BLOCK_END);
    block[NEXT_BLOCK] = next[0];
    block[NEXT_BLOCK + 1] = next[1];

    if (put_block(blockID, block)) {
        fprintf(stderr, "Error creating file block.\n");
        return -5;
    }

    return 0;
}

/*
 * readFile: reads a file
 *
 * @mem_pointer     String      where the contents of the file are read to
 * @blockID         Integer     location of the starting block of a file
 * @start           Integer     position in the file
 * @length          Integer     length of mem_pointer
 *
 * return  0:                   successful execution
 * return -1:                   invalid start value
 * return -2:                   error retrieving file block
 * return -3:                   error reading the file from that position
 */
int readFile(char* mem_pointer, int blockID, int start, int length) {
    if (start < 0) {
        fprintf(stderr, "Invalid start value.\n");
        return -1;
    }

    char* block;
    int nextBlockID;
    char next[2];

    /*
     * do
     *      get_block with blockID
     *      read nextBlockID from NEXT_BLOCK and NEXT_BLOCK + 1
     *      if start > DATA_SIZE && nextBlockID is BLOCK_END
     *          return with -3
     * while (start > DATA_SIZE && (blockID = nextBlockID) && (start -= DATA_SIZE))
     */

    do {
        block = malloc(BLOCK_SIZE);

        if (get_block(blockID, block)) {
            fprintf(stderr, "Error retrieving file block.\n");
            return -2;
        }

        next[0] = block[NEXT_BLOCK];
        next[1] = block[NEXT_BLOCK + 1];
        nextBlockID = decode_int(next);

        if (start > DATA_SIZE && nextBlockID == BLOCK_END) {
            fprintf(stderr, "Error reading the file from that position.\n");
            return -3;
        }

    } while (start > DATA_SIZE && (blockID = nextBlockID) && (start -= DATA_SIZE));

    // Next character in mem_pointer to read from
    int c = 0;

    // Current position in the file block
    int p = DATA_P + start;

    /*
     * do
     *      while p < BLOCK_SIZE and length > 0
     *          mem_pointer[c++] = block[p++]
     *          length--
     *
     *      if length > 0
     *          read blockID from NEXT_BLOCK and NEXT_BLOCK + 1
     *
     *          if blockID is BLOCK_END
     *              return with -3
     *
     *          get_block with blockID
     *          p = DATA_P
     * while length > 0
     *
     * return 0
     */

    do {
        while (p < BLOCK_SIZE && length > 0) {
            mem_pointer[c++] = block[p++];
            length--;
        }

        if (length > 0) {
            next[0] = block[NEXT_BLOCK];
            next[1] = block[NEXT_BLOCK + 1];
            blockID = decode_int(next);

            if (blockID == BLOCK_END) {
                fprintf(stderr, "Error reading the file from that position.\n");
                return -3;
            }

            block = malloc(BLOCK_SIZE);

            if (get_block(blockID, block)) {
                fprintf(stderr, "Error retrieving file block.\n");
                return -2;
            }

            p = DATA_P;
        }
    } while (length > 0);

    return 0;
}

/*
 * readDir: reads a directory
 *
 * @mem_pointer:    String      where the contents of the directory are read to
 * @blockID         Integer     the block id of the directory
 * @step            Integer     how far through the directory has been scanned
 *
 * return  1:                   Reached end of directory
 * return  0:                   successful execution
 * return -1:                   error retrieving file block
 */
int readDir(char* mem_pointer, int blockID, int step) {
    char* block = malloc(BLOCK_SIZE);

    if (get_block(blockID, block)) {
        fprintf(stderr, "Error retrieving file block.\n");
        return -1;
    }

    int final = 0;
    int p = 0;

    for (int i = ENTRY_START; i < BLOCK_SIZE; i += ENTRY_LENGTH) {
        if (block[i] != ENTRY_END && step-- == 0) {
            for (int j = i + NAME_P; j < i + MAX_DIRNAME; j++) {
                if (block[j] != '\0') {
                    mem_pointer[p++] = block[j];
                    final = p;
                }
            }

            mem_pointer[p++] = ' ';
        }
    }

    mem_pointer[final] = '\0';

    if (mem_pointer[0] == '\0') {
        return 1;
    }

    return 0;
}
