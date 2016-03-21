/* files.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 02/15/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file contains the definitions of functions that open and operate
 * on various types of files (input and output) for processing.
 * ======================================================================= */

#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include <stdarg.h>

#include "arguments.h"

/* ==================================================================
 * == FUNCTIONS
 * =============================================================== */

/* Open a file for either reading or writing data. If the file cannot
 * be opened, exit with an error dialog. Otherwise, return the file
 * pointer to the calling function.
 */
FILE *openFile(const char fileName[], const argtype_t fileType);


/* Read exactly one line (until next newline) from the file pointed
 * to by *fp.
 */
char *readLine(FILE *fp);


/* Write `i` string's received as input to the file pointed to by 
 * `*fp`. Exit the function by printing a newline `\n`. 
 */
void writeLine(FILE *fp, size_t i, ...);


/* This is basically a rewrite of the fgets() function in clib, the
 * only difference being that it does NOT add the `\n` newline to the
 * string it reads from the file.
 */
char *fgetline(char *s, int n, FILE *file);


#endif