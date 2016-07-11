/* 
 * files.h -- opening, reading, and writing to files
 * 
 * Created by PAT GAFFNEY on 06/17/2016.
 * Last modified on 07/10/2016.
 * 
 *********ultrapatbeams*/

#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include "strings.h"


/******************************************************************
 * open_file() -- open a file
 *
 * const char *fileName -- name of file to be opened
 *
 * @throws -- throw_file_opening_error() if file cannot be opened
 * @return -- pointer to an open file stream
 ******************************************************************/
FILE *open_file(const char *fileName);


/******************************************************************
 * read_line() -- read from inputFile until a newline is reached
 *
 * FILE *inputFile -- file from which characters are read
 *
 * @return -- readstring_t structure as read from file
 ******************************************************************/
readstring_t read_line(FILE *inputFile);


/******************************************************************
 * close_file() -- close a file
 *
 * FILE *stream -- file stream to be closed
 *
 * @noreturn -- control returned to the caller
 ******************************************************************/
void close_file(FILE *io);

#endif