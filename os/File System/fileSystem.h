/*
 * fileSystem.h
 *
 */

#define BLOCKS 512
#define BLOCK_SIZE 128

// Opens a file descriptor to the file.
int sfs_open(char* pathname);

int sfs_read(int fd, int start, int length, char* mem_pointer);

int sfs_write(int fd, int start, int length, char* mem_pointer);

int sfs_readdir(int fd, char* mem_pointer);

int sfs_close(int fd);

// Deletes a file.
int sfs_delete(char* pathname);

// Creates a file.
int sfs_create(char* pathname, int type);

// Gets the size of a file.
int sfs_getsize(char* pathname);

// Gets the type of a file.
int sfs_gettype(char* pathname);

int sfs_initialize(int erase);
