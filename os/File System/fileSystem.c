/*
 * fileSystem.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blockio.h"
#include "entry.h"
#include "fControl.h"
#include "fileSystem.h"
#include "openFiles.h"
#include "pathUtils.h"

/* sfs_open: Opens a file descriptor to the file.
 *
 * @pathname    String      a path to a file.
 *
 * return  fd (>= 0):       successful execution
 * return -1:               error parsing the path
 * return -2:               error traversing the file system
 * return -3:               error adding block id to the file open table
 */
int sfs_open(char* pathname) {
    char** path = malloc(MAX_PATH);

    for (int i = 0; i < MAX_PATH; i++) {
        path[i] = malloc(MAX_DIRNAME);
    }

    // Parse the pathname
    if (parsePath(path, pathname)) {
        fprintf(stderr, "Error parsing the path.\n");
        return -1;
    }

    int blockID;

    // Traverse the file system for blockID of the last component
    if (traverse(&blockID, path)) {
        fprintf(stderr, "Error traversing the file system.\n");
        return -2;
    }

    int fd;

    // Add the opened block to the file open table
    if (add(&fd, blockID)) {
        fprintf(stderr, "Error adding block id to the file open table.\n");
        return -3;
    }

    return fd;
}

/*
 * sfs_read: Copies data stored in a regular file into a specified memory pointer.
 *
 * @fd              Integer     the file descriptor pointing to the file to read from
 * @start           Integer     the starting char to read from the file
 * @length          Integer     how many chars to read from the file
 * @mem_pointer     String      the string to read into
 *
 * return  1:                   successful execution
 * return -1:                   error finding opened block id from the file open table
 * return -2:                   error getting file type
 * return -3:                   file is not a regular file
 * return -4:                   error reading file
 */
int sfs_read(int fd, int start, int length, char* mem_pointer) {
    int blockID;

    // Find the block id corresponding to the file descriptor from the file open table
    if (find(&blockID, fd)) {
        fprintf(stderr, "Error finding opened block id from the file open table.\n");
        return -1;
    }

    int type;

    if (getType(&type, blockID)) {
        fprintf(stderr, "Error getting file type.\n");
        return -2;
    }

    if (type != FILE) {
        fprintf(stderr, "File is not a regular file.\n");
        return -3;
    }

    for (int i = 0; i < MAX_IO_LENGTH + 1; i++) {
        mem_pointer[i] = '\0';
    }

    if (readFile(mem_pointer, blockID, start, length)) {
        fprintf(stderr, "Error reading file.\n");
        return -4;
    }

    return 1;
}

/*
 * sfs_write: Writes data stored in a memory location into a file.
 *
 * @fd              Integer     the file descriptor pointing to the file to read from
 * @start           Integer     the starting char to read from the file
 * @length          Integer     how many chars to read from the file
 * @mem_pointer     String      the string to read into
 *
 * return  1:                   successful execution
 * return -1:                   error finding opened block id from the file open table
 * return -2:                   error writing file
 */
int sfs_write(int fd, int start, int length, char* mem_pointer) {
    int blockID;

    // Find the block id corresponding to the file descriptor from the file open table
    if (find(&blockID, fd)) {
        fprintf(stderr, "Error finding opened block id from the file open table.\n");
        return -1;
    }

    if (writeFile(mem_pointer, blockID, start, length)) {
        fprintf(stderr, "Error writing file.\n");
        return -2;
    }

    return 1;
}

/*
 * sfs_readdir: Reads a directory's contents into a memory pointer.
 *
 * @fd              Integer     the file descriptor pointing to the file to read from
 * @mem_pointer     String      the string to read into
 *
 * return  1:                   successful execution
 * return -1:                   error finding opened block id from the file open table
 * return -2:                   error reading directory contents
 */
int sfs_readdir(int fd, char* mem_pointer) {
    int blockID;

    for (int i = 0; i < MAX_IO_LENGTH + 1; i++) {
        mem_pointer[i] = '\0';
    }

    // Find the block id corresponding to the file descriptor from the file open table
    if (find(&blockID, fd)) {
        fprintf(stderr, "Error finding opened block id from the file open table.\n");
        return -1;
    }

    if (readDir(mem_pointer, blockID)) {
        fprintf(stderr, "Error reading directory contents.\n");
        return -2;
    }

    return 1;
}

/*
 * sfs_close: Closes a file descriptor.
 *
 * @fd          Integer     the file descriptor pointing to the file to read from
 *
 * return  1:               successful execution
 * return -1:               error deleting entry in the file open table corresponding to fd
 */
int sfs_close(int fd) {

    // Delete entry in the file open table
    if (delete(fd)) {
        fprintf(stderr, "Error deleting entry in the file open table corresponding to the file descriptor %d.\n", fd);
        return -1;
    }

    return 1;
}

/* sfs_delete: Deletes a file.
 *
 * @pathname    String      a path to a file.
 *
 * return  1:               successful execution
 * return -1:               error parsing the path
 * return -2:               error finding the parent directory of the file
 * return -3:               error traversing the file system
 * return -4:               error getting the file type
 * return -5:               error deleting directory
 * return -6:               error deleting file
 */
int sfs_delete(char* pathname) {
    char** path = malloc(MAX_PATH);

    for (int i = 0; i < MAX_PATH; i++) {
        path[i] = malloc(MAX_DIRNAME);
    }

    // Parse the pathname
    if (parsePath(path, pathname)) {
        fprintf(stderr, "Error parsing the path.\n");
        return -1;
    }

    char** parent = malloc(MAX_PATH);

    for (int i = 0; i < MAX_PATH - 1; i++) {
        parent[i] = malloc(MAX_DIRNAME);
    }

    // Find the parent directory
    if (dirname(parent, path)) {
        fprintf(stderr, "Error finding the parent directory of the file.\n");
        return -2;
    }

    int blockID;

    // Traverse the file system for blockID of the last component
    if (traverse(&blockID, path)) {
        fprintf(stderr, "Error traversing the file system.\n");
        return -3;
    }

    int type;

    // Get the file type of the file component
    if (getTypeFromFCB(&type, blockID, path[arrayLen(path) - 1])) {
        fprintf(stderr, "Error getting the file type.\n");
        return -4;
    }

    if (type == 1) {
        if (deleteDir(blockID, path[arrayLen(path) - 1])) {
            fprintf(stderr, "Error deleting directory.\n");
            return -5;
        }
    } else if (type == 0) {
        if (deleteFile(blockID, path[arrayLen(path) - 1])) {
            fprintf(stderr, "Error deleting file.\n");
            return -6;
        }
    }

    return 1;
}

/* sfs_create: Creates a file.
 * special thanks: http://cboard.cprogramming.com/c-programming/67987-passing-2d-array-reference.html#post483081
 *
 * @pathname    String      a path to a file.
 * @type        Integer     type of file.
 *                              - 0 if regular file
 *                              - 1 if directory
 *
 * return  1:               successful execution
 * return -1:               error parsing the path
 * return -2:               error finding the parent directory of the file
 * return -3:               error traversing the file system
 * return -4:               error creating the file control block
 * return -5:               error creating file
 */
int sfs_create(char* pathname, int type) {
    char** path = malloc(MAX_PATH);

    for (int i = 0; i < MAX_PATH; i++) {
        path[i] = malloc(MAX_DIRNAME);
    }

    // Parse the pathname
    if (parsePath(path, pathname)) {
        fprintf(stderr, "Error parsing the path.\n");
        return -1;
    }

    char** parent = malloc(MAX_PATH);

    for (int i = 0; i < MAX_PATH - 1; i++) {
        parent[i] = malloc(MAX_DIRNAME);
    }

    // Find the parent directory
    if (dirname(parent, path)) {
        fprintf(stderr, "Error finding the parent directory of the file.\n");
        return -2;
    }

    int parentBlock;

    // Traverse the file system for blockID of the directory containing the component
    if (traverse(&parentBlock, parent)) {
        fprintf(stderr, "Error traversing the file system.\n");
        return -3;
    }

    if (type == 1) {
        if (createFCB(parentBlock, path[arrayLen(path) - 1])) {
            fprintf(stderr, "Error creating the file control block.\n");
            return -4;
        }
    } else if (type == 0) {
        if (createFile(parentBlock, path[arrayLen(path) - 1])) {
            fprintf(stderr, "Error creating file.\n");
            return -5;
        }
    }

    return 1;
}

/* sfs_getsize: Gets the size of a file.
 *
 * @pathname    String      a path to a file.
 *
 * return  size (> -1):     successful execution, file size
 * return           -1:     error parsing the path
 * return           -2:     error finding the parent directory of the file
 * return           -3:     error traversing the file system
 * return           -4:     error getting file size
 */
int sfs_getsize(char* pathname) {
    char** path = malloc(MAX_PATH);

    for (int i = 0; i < MAX_PATH; i++) {
        path[i] = malloc(MAX_DIRNAME);
    }

    // Parse the pathname
    if (parsePath(path, pathname)) {
        fprintf(stderr, "Error parsing the path.\n");
        return -1;
    }

    int blockID;

    // Traverse the file system for blockID of the last component
    if (traverse(&blockID, path)) {
        fprintf(stderr, "Error traversing the file system.\n");
        return -3;
    }

    int size;

    if (getSize(&size, blockID)) {
        fprintf(stderr, "Error getting file size.\n");
        return -4;
    }

    return size;
}

/* sfs_gettype: Gets the type of a file.
 *
 * @pathname    String      a path to a file.
 *
 * return  0:               successful execution, file is a regular file
 * return  1:               successful execution, file is a directory
 * return -1:               error parsing the path
 * return -3:               error traversing the file system
 * return -4:               error getting the file type
 */
int sfs_gettype(char* pathname) {
    char** path = malloc(MAX_PATH);

    for (int i = 0; i < MAX_PATH; i++) {
        path[i] = malloc(MAX_DIRNAME);
    }

    // Parse the pathname
    if (parsePath(path, pathname)) {
        fprintf(stderr, "Error parsing the path.\n");
        return -1;
    }

    int blockID;

    // Traverse the file system for blockID of the last component
    if (traverse(&blockID, path)) {
        fprintf(stderr, "Error traversing the file system.\n");
        return -2;
    }

    int type;

    // Get the file type of the file component
    if (getType(&type, blockID)) {
        fprintf(stderr, "Error getting the file type.\n");
        return -3;
    }

    return type;
}

/* sfs_initialize: Initializes the file system.
 *
 * @erase       Integer     If equal to 1 to erase disk while initializing,
 *                          otherwise just initialize the disk
 *
 * return  1:               successful execution
 */
int sfs_initialize(int erase) {
    if (erase == 1) {
        char* empty = malloc(BLOCK_SIZE);
        empty[0] = FREE;

        for (int i = 0; i < BLOCKS; i++) {
            put_block(i, empty);
        }
    }

    sfs_create("/", 1);

    return 1;
}
