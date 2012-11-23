/*
 * entry.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blockio.h"
#include "entry.h"
#include "fControl.h"
#include "fileSystem.h"
#include "pathUtils.h"
#include "storeInt.h"

/*
 * getFreeBlock: find a free block in the file system
 *
 * @blockID     Integer Pointer     location of the next free block
 *
 * return  0:                       successful execution
 * return -1:                       error finding free block
 * return -2:                       error reading block
 */
int getFreeBlock(int* blockID) {
    char* block = malloc(BLOCK_SIZE);

    for (int i = ROOT_BLOCKID; i < BLOCKS; i++) {
        if (get_block(i, block)) {
            fprintf(stderr, "Error reading block %d.\n", i);
            return -2;
        }

        if (block[0] == FREE) {
            *blockID = i;
            return 0;
        }
    }

    return -1;
}

/*
 * getEntryPoint: find the next position in the file control block where entries can be added.
 *
 * @position    Integer Pointer     position in the file control block
 * @fcb         String              the file control block
 *
 * return  0:                       successful execution
 * return -1:                       error finding available entry point in the file control block
 */
int getEntryPoint(int* position, char* fcb) {

    for (int i = ENTRY_START; i < BLOCK_SIZE; i += ENTRY_LENGTH) {
        if (fcb[i] == ENTRY_END) {
            *position = i;
            return 0;
        }
    }

    return -1;
}

/*
 * addEntry: Adds an entry to a fcb.
 *
 * @start:      Integer Pointer     location of the starting block
 * @fcBlockID:  Integer             the target fcb id
 * @name:       String              name of the entry
 * @type:       Integer             type of the entry
 *
 * return  0:                       successful execution
 * return -1:                       file already exists in directory
 * return -2:                       error retrieving the parent file control block
 * return -3:                       error looking for a free block
 * return -4:                       error finding entry in the file control block
 * return -5:                       error creating file control block
 */
int addEntry(int* start, int fcBlockID, char* name, int type) {
    /* best case:
     *      block: [1, a, b, c, d, e, f, ... ]
     * average case:
     *      block: [1, a, b, c, 0, 0, 0, ... ]
     * worst case:
     *      block: [1, 0, 0, 0, 0, 0, 0, ... ]
     *
     * Therefore, 7 chars must be skipped before any useful data is reached.
     */

    if (getStart(malloc(1), fcBlockID, name) != -2) {
        fprintf(stderr, "File already exists in directory.\n");
        return -1;
    }

    char* fcb = malloc(BLOCK_SIZE);

    if (get_block(fcBlockID, fcb)) {
        fprintf(stderr, "Error retrieving the parent file control block.\n");
        return -2;
    }

    if (getFreeBlock(start)) {
        fprintf(stderr, "Error looking for a free block.\n");
        return -3;
    }

    // Position in the file control block
    int position;

    if (getEntryPoint(&position, fcb)) {
        fprintf(stderr, "Error finding entry in the file control block.\n");
        return -4;
    }

    /*
     * order of file attributes:
     *      type    name    start
     */

    fcb[position++] = type;

    for (int i = 0; i < strlen(name); i++) {
        fcb[i + position] = name[i];
    }

    position += MAX_DIRNAME - 1;

    char* _start = encode_int(*start);

    for (int i = 0; i < START; i++) {
        fcb[i + position] = _start[i];
    }

    position += START;

    if (fcb[position + 1] == '\0') {
        fcb[position] = ENTRY_END;
    }

    if (put_block(fcBlockID, fcb)) {
        fprintf(stderr, "Error creating file control block.\n");
        return -5;
    }

    return 0;
}

/*
 * removeEntry: Removes an entry from a fcb.
 *
 * @start:      Integer Pointer     location of the starting block
 * @fcBlockID:  Integer             the target fcb id
 * @name:       String              name of the entry
 *
 * return  0:                       successful execution
 * return -1:                       error retrieving the parent file control block
 * return -2:                       error finding entry in the file control block
 * return -3:                       error creating file control block
 */
int removeEntry(int* start, int fcBlockID, char* name) {
    char* fcb = malloc(BLOCK_SIZE);

    if (get_block(fcBlockID, fcb)) {
        fprintf(stderr, "Error retrieving the parent file control block.\n");
        return -1;
    }

    for (int i = ENTRY_START; i < BLOCK_SIZE; i += ENTRY_LENGTH) {
        char* line = &fcb[i];
        char entryName[MAX_DIRNAME - 1];
        strncpy(entryName, &line[NAME_P], MAX_DIRNAME - 1);
        entryName[MAX_DIRNAME - 1] = '\0';

        if (!strcmp(entryName, name)) {
            char _start[2];
            _start[0] = line[START_P];
            _start[1] = line[START_P + 1];

            *start = decode_int(_start);

            fcb[i] = ENTRY_END;

            for (int j = i + 1; j < i + ENTRY_LENGTH + 1; j++) {
                fcb[j] = '\0';
            }

            if (put_block(fcBlockID, fcb)) {
                fprintf(stderr, "Error creating file control block.\n");
                return -3;
            }

            return 0;
        }
    }

    fprintf(stderr, "Error finding entry in the file control block.\n");
    return -2;
}

/*
 * getType: Get file type of a file from the file control block
 *
 * precondition: length of name is of valid length
 *
 * @type        Integer Pointer     file type
 * @fcBlockID   Integer             location of the file control block
 *
 * return  0:                       successful execution
 * return -1:                       error retrieving the file control block
 * return -2:                       error finding entry in the file control block
 */
int getType(int* type, int fcBlockID, char* name) {
    if (fcBlockID == ROOT_BLOCKID && !strcmp(name, ROOT)) {
        *type = DIRECTORY;
        return 0;
    } else {
        char* fcb = malloc(BLOCK_SIZE);

        if (get_block(fcBlockID, fcb)) {
            fprintf(stderr, "Error retrieving the file control block.\n");
            return -1;
        }

        for (int i = ENTRY_START; i < BLOCK_SIZE; i += ENTRY_LENGTH) {
            char* line = &fcb[i];
            char entryName[MAX_DIRNAME - 1];
            strncpy(entryName, &line[NAME_P], MAX_DIRNAME - 1);
            entryName[MAX_DIRNAME - 1] = '\0';

            if (!strcmp(entryName, name)) {
                *type = line[TYPE_P];
                return 0;
            }
        }
    }

    fprintf(stderr, "Error finding entry in the file control block.\n");
    return -2;
}

/*
 * getStart: Get the starting block of the file path component from the file control block.
 *
 * precondition: length of name is of valid length
 *
 * @block       Integer Pointer     starting block id of the file component
 * @fcBlock     Integer             the file control block id
 * @name        String              the file component
 *
 * return  0:                       successful execution
 * return -1:                       error retrieving the file control block
 * return -2:                       error finding entry in the file control block
 */
int getStart(int* blockID, int fcBlockID, char* name) {
    if (fcBlockID == ROOT_BLOCKID && !strcmp(name, ROOT)) {
        *blockID = ROOT_BLOCKID;
        return 0;
    }

    char* fcb = malloc(BLOCK_SIZE);

    if (get_block(fcBlockID, fcb)) {
        fprintf(stderr, "Error retrieving the file control block.\n");
        return -1;
    }

    for (int i = ENTRY_START; i < BLOCK_SIZE; i += ENTRY_LENGTH) {
        char* line = &fcb[i];
        char entryName[MAX_DIRNAME - 1];
        strncpy(entryName, &line[NAME_P], MAX_DIRNAME - 1);
        entryName[MAX_DIRNAME - 1] = '\0';

        if (!strcmp(entryName, name)) {
            char start[2];
            start[0] = line[START_P];
            start[1] = line[START_P + 1];

            *blockID = decode_int(start);
            return 0;
        }
    }

    return -2;
}

/*
 * getSize: Get file size of a regular file.
 *
 * precondition: length of name is of valid length
 *
 * @size      Integer Pointer   file size
 * @blockID   Integer           location of the starting block of a file
 *
 * return  0:                   successful execution
 * return -1:                   error retrieving the file control block
 */
int getSize(int* size, int blockID) {
    *size = 0;
    char* block;
    char next[2];

    do {
        block = malloc(BLOCK_SIZE);

        if (get_block(blockID, block)) {
            fprintf(stderr, "Error retrieving the file control block.\n");
            return -1;
        }

        next[0] = block[NEXT_BLOCK];
        next[1] = block[NEXT_BLOCK + 1];

        blockID = decode_int(next);
    } while (blockID != BLOCK_END && (*size += BLOCK_SIZE - 3));

    int p = DATA_P;

    while (block[p++] != '\0' && ((*size)++)) {
    }

    return 0;
}
