/*
 * openFiles.h
 *
 */

#define MAX_OPEN_FILES 512

struct table {
    int fd[MAX_OPEN_FILES][2];
    int length;
};

struct table openTable;

// Finds the block id corresponding to a file descriptor.
int find(int* blockID, int fd);

// Deletes the entry in the priority queue corresponding to the file descriptor.
int delete(int fd);

// Adds an entry in the priority queue by assigning a file descriptor to the block id.
int add(int* fd, int blockID);

// Deletes all entries in the priority queue with value blockID.
void deleteAll(int blockID);
