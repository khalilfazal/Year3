/*
 * openFiles.c
 *
 */

#include <stdio.h>
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
        if (openTable.fd[i][0] == fd) {
            *blockID = i;
            return 0;
        }
    }

    fprintf(stderr, "Unable to find the corresponding block id.\n");
    return 1;
}

/*
 * delete: Deletes the entry in the priority queue corresponding to the file descriptor.
 *
 * @fd          Integer             the file descriptor
 *
 * return 0:                        successful execution
 * return 1:                        file descriptor does not exist in the priority queue
 */
int delete(int fd) {
    if (fd == openTable.length - 1) {
        openTable.fd[fd][0] = 0;
    } else if (fd > (openTable.length - 1) || (fd < 0)) {
        fprintf(stderr, "File descriptor does not exist in the priority queue.\n");
        return 1;
    } else {
        //Picks the vertex
        for (int i = fd; i < openTable.length; i++) {
            openTable.fd[i][0] = openTable.fd[i + 1][0];
            openTable.fd[i][1] = openTable.fd[i + 1][1];
        }
        openTable.length--;
    }

    return 0; //This means it successfully deleted whatever index you wanted deleted
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

    for (int i = 1; i < openTable.length; i++) {
        //These two lines move everything 'up' (to the right)
        //It starts from the first
        openTable.fd[i + 1][0] = openTable.fd[i][0];
        openTable.fd[i + 1][1] = openTable.fd[i][1];
    }

    openTable.fd[0][0] = openTable.fd[1][0] + 1;
    openTable.fd[0][1] = blockID;

    *fd = openTable.fd[0][0];

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
