/***** 
 * files.h -- opening, closing, and reading input from files
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-17
 * @modified    2016-10-04
 * 
 ************************************************************************/

#ifndef __MD_FILES_H__
#define __MD_FILES_H__

#include <stdio.h>

#include "strings.h"

/************************************************************************
 * Opening & Closing Files
 ************************************************************************/

/** Open a file stream for some given file access mode. **/
FILE *open_file(const char *fileName, const char *mode);

/** Close a file stream, but only if the file stream exists. **/
void close_file(FILE *io);


/************************************************************************
 * Reading Input From Files
 ************************************************************************/

/** Read all bytes from a supplied input file stream. **/
String *read_all_input_bytes(FILE *ifp);

#endif
