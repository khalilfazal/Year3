/*
 * pathUtils.c
 *
 */

#include <stdio.h>
#include <string.h>
#include "fControl.h"
#include "pathUtils.h"

/*
 * parsePath: Parses a path string into an array of path components.
 * Each string is a directory name.
 * The path is split into tokens which are separated by delimiters (‘/’ in this case).
 *
 * @path        Array of Strings    The parsed path.
 * @pathname    String              The unparsed path.
 *
 * return 0:                        successful execution
 * return 1:                        pathname is empty
 * return 2:                        pathname is not absolute
 * return 3:                        pathname contains consecutive forward slashes ("//")
 * return 4:                        component contains more than six char
 */
int parsePath(char** path, char* pathname) {
    // If pathname is an empty string, return with an error.
    if (pathname[0] == '\0') {
        fprintf(stderr, "Pathname is empty.\n");
        return 1;
    }

    // If the first char of pathname is not a ‘/’, return with an error.
    if (pathname[0] != '/') {
        fprintf(stderr, "Pathname is not absolute.\n");
        return 2;
    }

    // If the pathname contains consecutive forward slashes ("//").
    for (int i = 0; i < strlen(pathname) - 1; i++) {
        if (pathname[i] == '/' && pathname[i + 1] == '/') {
            fprintf(stderr, "Pathname contains consecutive forward slashes (\"//\").\n");
            return 3;
        }
    }

    // Current level in the path
    int level = 0;

    // Current position in file's name
    int filePos = 0;

    // Add root to the path
    path[level][filePos++] = '/';
    path[level++][filePos] = '\0';

    filePos = 0;

    // Loop through each char in the pathname, starting from the second char.
    for (int i = 1; i < strlen(pathname); i++) {

        // If the current char in the pathname is a forward slash.
        if (pathname[i] == '/') {

            // End the string with a null char.
            path[level++][filePos] = '\0';

            // Reset the current index of the file name.
            filePos = 0;
        } else {

            // If a component contains more than six char.
            if (filePos > 6) {
                fprintf(stderr, "The length of a pathname component must be limited to six characters\n");
                return 4;
            }

            // Add the current char to current file's name.
            path[level][filePos++] = pathname[i];
        }
    }

    path[level][filePos] = '\0';

    if (filePos > 0) {
        level++;
    }

    path[level][0] = '\0';
    return 0;
}

/*
 * traverse: Traverse through the file system and retrieve the blockID of the last component.
 *
 * @blockID     Integer             blockID of the last component
 * @path        Array of Strings    a list of path components to traverse
 *
 * return  0:                       successful execution
 * return -1:                       error getting the file type
 * return -2:                       error in interpreting a regular file as a directory
 * return -3:                       error retrieving the path component from the file control block
 */
int traverse(int* blockID, char** path) {

    // Start traversing from the root block.
    *blockID = ROOT_BLOCKID;

    for (int i = 1; i < arrayLen(path); i++) {
        int type;

        // Get the file type of the file component
        if (getType(&type, *blockID, path[i])) {
            fprintf(stderr, "Error getting the file type.\n");
            return -1;
        }

        // If a component in the middle of a path is not a directory
        if (type == 0 && i < arrayLen(path) - 1) {
            fprintf(stderr, "Error in interpreting a regular file as a directory.\n");
            return -2;
        }

        if (getStart(blockID, *blockID, path[i])) {
            fprintf(stderr, "Error retrieving ./%s from the file control block of ", path[i]);

            if (i > 0) {
                fprintf(stderr, "%s", path[i - 1]);
            } else {
                fprintf(stderr, "/");
            }

            fprintf(stderr, ".\n");
            return -3;
        }
    }

    return 0;
}

/*
 * dirname: Strip last component from file name.
 * The parent directory of the root directory is the root directory
 *
 * @dir     Array of Strings      the path to the parent directory of a file component
 * @path    Array of Strings      the path to a file component
 */
int dirname(char** dir, char** path) {
    int length = arrayLen(path);

    if (length == 1) {
        *dir = *path;
        dir[length][0] = '\0';
    } else {
        for (int i = 0; i < length - 1; i++) {
            dir[i] = path[i];
        }

        dir[length - 1][0] = '\0';
    }

    return 0;
}

/*
 * arrayLen: Finds the length of an array terminated with a null char.
 * special thanks: http://stackoverflow.com/a/232882
 *
 * @array       Array of Strings    the array of unknown length
 * return int:  Integer             the length of the array
 */
int arrayLen(char** array) {
    int output = 0;
    int i = 0;

    while (array[i++][0] != '\0') {
        output++;
    }

    return output;
}
