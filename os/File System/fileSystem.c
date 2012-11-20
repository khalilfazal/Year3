/*
 * fileSystem.c
 *
 */

#include <stdio.h>
#include "fControl.h"
#include "fileSystem.h"
#include "pathUtils.h"

/* sfs_open: Opens a file descriptor to the file.
 *
 * @pathname    String      a path to a file.
 *
 * return  1:               successful execution
 * return -1:               error parsing the path
 * return -2:               error traversing the file system
 */
int sfs_open(char* pathname) {
    char path[MAX_PATH][MAX_DIRNAME];

    // Parse the pathname
    if (!parsePath(path, pathname)) {
        fprintf(stderr, "Error parsing the path.\n");
        return -1;
    }

    int blockID;

    // Traverse the file system for blockID of the last component
    if (!traverse(&blockID, pathname)) {
        fprintf(stderr, "Error traversing the file system.");
        return -2;
    }

    return 1;
}

int sfs_read(int fd, int start, int length, char* mem_pointer) {
    return 1;
}

int sfs_write(int fd, int start, int length, char* mem_pointer) {
    return 1;
}

int sfs_readdir(int fd, char* mem_pointer) {
    return 1;
}

int sfs_close(int fd) {
    return 1;
}

/* sfs_delete: Deletes a file.
 *
 * @pathname    String      a path to a file.
 *
 * return  1:               successful execution
 * return -1:               error parsing the path
 * return -2:               error traversing the file system
 */
int sfs_delete(char* pathname) {
    char path[MAX_PATH][MAX_DIRNAME];

    // Parse the pathname
    if (!parsePath(path, pathname)) {
        fprintf(stderr, "Error parsing the path.\n");
        return -1;
    }

    int blockID;

    // Traverse the file system for blockID of the last component
    if (!traverse(&blockID, pathname)) {
        fprintf(stderr, "Error traversing the file system.");
        return -2;
    }

    return 1;
}

/* sfs_create: Creates a file.
 *
 * @pathname    String      a path to a file.
 * @type        Integer     type of file.
 *                              - 0 if regular file
 *                              - 1 if directory
 *
 * return  1:               successful execution
 * return -1:               error parsing the path
 * return -2:               error traversing the file system
 * return -3:               error creating the file control block
 */
int sfs_create(char* pathname, int type) {
    char path[MAX_PATH][MAX_DIRNAME];

    // Parse the pathname
    if (!parsePath(path, pathname)) {
        fprintf(stderr, "Error parsing the path.\n");
        return -1;
    }

    int blockID;

    // Traverse the file system for blockID of the last component
    if (!traverse(&blockID, path)) {
        fprintf(stderr, "Error traversing the file system.");
        return -2;
    }

    if (type == 1) {
        if (!createFCB(blockID, path[LENGTH(path)-1])) {
            fprintf(stderr, "Error creating the file control block");
            return -3;
        }
    }

    return 1;
}

/* sfs_getsize: Gets the size of a file.
 *
 * @pathname    String      a path to a file.
 *
 * return  1:               successful execution
 * return -1:               error parsing the path
 * return -2:               error traversing the file system
 */
int sfs_getsize(char* pathname) {
    char path[MAX_PATH][MAX_DIRNAME];

    // Parse the pathname
    if (!parsePath(path, pathname)) {
        fprintf(stderr, "Error parsing the path.\n");
        return -1;
    }

    int blockID;

    // Traverse the file system for blockID of the last component
    if (!traverse(&blockID, path)) {
        fprintf(stderr, "Error traversing the file system.");
        return -2;
    }

    return 1;
}

/* sfs_gettype: Gets the type of a file.
 *
 * @pathname    String      a path to a file.
 *
 * return  1:               successful execution
 * return -1:               error parsing the path
 * return -2:               error traversing the file system
 */
int sfs_gettype(char* pathname) {
    char path[MAX_PATH][MAX_DIRNAME];

    // Parse the pathname
    if (!parsePath(path, pathname)) {
        fprintf(stderr, "Error parsing the path.\n");
        return -1;
    }

    int blockID;

    // Traverse the file system for blockID of the last component
    if (!traverse(&blockID, path)) {
        fprintf(stderr, "Error traversing the file system.");
        return -2;
    }

    return 1;
}

int sfs_initialize(int erase) {
    sfs_create("/", 1);

    return 1;
}
