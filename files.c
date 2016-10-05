/***** 
 * files.c -- opening, closing, and reading input from files
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-17
 * @modified    2016-10-05
 * 
 ************************************************************************/

#include <stdbool.h>
#include <stdio.h>

#include "errors.h"
#include "files.h"
#include "strings.h"

/************************************************************************
 * Opening & Closing Files
 ************************************************************************/

/*****
 * Open a file stream for some given file access mode.
 *
 * ARGUMENTS
 *  fileName    the name of the file to be opened (a C-string)
 *  type        the mode by which the file should be opened: "r", "w"
 *
 * RETURNS
 *  A pointer to the file stream.
 *****/
FILE *open_file(const char *fileName, const char *mode)
{
    FILE *filePtr = NULL;
    filePtr = fopen(fileName, mode);
    if (!filePtr) throw_fatal_fopen_error(fileName);
    return filePtr;
}


/*****
 * Close a file stream, but only if the file stream exists.
 *****/
void close_file(FILE *io)
{
    if (io) fclose(io);
}


/************************************************************************
 * Reading Input From Files
 ************************************************************************/

/*****
 * Read all bytes from a supplied input file stream.
 *
 *  Bytes are read into a String buffer that is reallocated by an order
 *  of 5120 bytes in order to make room for the entire file.
 *
 * ARGUMENTS
 *  ifp     Input file stream (must be opened for reading).
 *
 * RETURNS
 *  A String node initialized with the bytes read from file, the memory
 *  allocated for this node (in bytes), and the number of bytes actually
 *  read from the input file stream.
 *****/
String *read_all_input_bytes(FILE *ifp)
{
    int ret   = 0;      /* The return value from fread(). */
    int lim   = 5120;   /* Max number of bytes to allocate for. */
    int order = 1;      /* Multiplier for lim if we realloc() memory. */    
    String *bytes = init_string(lim);
    
    while (true) {
        ret = fread(bytes->data + bytes->length, 1, 
                    bytes->allocd - bytes->length, ifp);
        bytes->length += ret;
        
        if (feof(ifp)) break;
        else {
            lim += lim * ++order;
            realloc_string(bytes, lim);
        }
    }
    bytes->data[bytes->length] = '\0';
    return bytes;
}
