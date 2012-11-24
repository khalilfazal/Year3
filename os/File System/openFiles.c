/*
 * openFiles.c
 *
 */

#include <stdio.h>
#include <string.h>
#include "openFiles.h"

/*
 * find: Finds the block id corresponding to a file descriptor.
 *
 * @blockID     Integer Pointer     the found block id
 * @fd          Integer             the file descriptor
 *
 * return 0:                        successful execution
 * return 1:                        unable to find the corresponding block id
 */
int find(int* blockID, int fd) {
    for (int i = 0; i < openTable.length; i++) {
        if (openTable.fd[i][0] == fd + 1) {
            *blockID = openTable.fd[i][1];
            return 0;
        }
    }

    fprintf(stderr, "Unable to find the corresponding block id.\n");
    return 1;
}

/*
 * delete: Deletes the entry in the priority queue corresponding to the file descriptor.
 *
 * @fd          Integer     the file descriptor
 *
 * return  0:               successful execution
 * return -1:               file descriptor is less than zero
 * return -2:               error finding file descriptor
 */
int delete(int fd) {
    if (fd < 0) {
        fprintf(stderr, "File descriptor is less than zero.\n");
        return -1;
    }

    for (int i = 0; i < openTable.length; i++) {
        if (openTable.fd[i][0] == fd + 1) {
            for (int j = i; j < openTable.length; j++) {
                memcpy(openTable.fd[j], openTable.fd[j + 1], 2);
            }

            openTable.length--;
            return 0;
        }
    }

    fprintf(stderr, "Error finding file descriptor.\n");
    return -2;
}

/*
 * add: Adds an entry in the priority queue by assigning a file descriptor to the block id.
 *
 * @fd          Integer Pointer     the assigned file descriptor
 * @blockID     Integer             the block id to be added to the open file table
 *
 * return 0:                        successful execution
 */
int add(int* fd, int blockID) {
    openTable.length++;

    for (int i = openTable.length - 2; i > -1; i--) {
        memcpy(openTable.fd[i + 1], openTable.fd[i], 2);
    }

    openTable.fd[0][0] = openTable.fd[1][0] + 1;
    openTable.fd[0][1] = blockID;

    *fd = openTable.fd[0][0] - 1;

    return 0;
}

/*
 * deleteAll: Deletes all entries in the priority queue with value blockID
 *
 * @blockID     Integer     the block id
 *
 */
void deleteAll(int blockID) {

    for (int i = 0; i < openTable.length; i++) {
        if (openTable.fd[i][1] == blockID) {
            delete(i);
        }
    }
}
