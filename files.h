/***** 
 * files.h -- opening, reading, and writing to files
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-17
 * @modified    2016-09-30
 * 
 ************************************************************************/

#ifndef FILES_H
#define FILES_H

#include <stdio.h>

#include "strings.h"

/* Open a file stream for reading */
FILE *open_file(const char *fileName, const char *type);

/* Read a line of text from inputFile. */
String *read_line(FILE *inputFile);

/* Close a file stream. */
void close_file(FILE *io);

#endif