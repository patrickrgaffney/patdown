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
#include <stdbool.h>

#include "arguments.h"


/* open_file(const char *, const argtype_t)
 * =======================================================================
 * Open a file for reading (type "r") or writing (type "w") based on the
 * argtype_t passed to the funtion.
 *
 * Return a pointer to the opened file.
 * NOTE: If the file cannot be opened, then the program will exit with an
 *       error message and code.
 * ======================================================================= */
FILE *open_file(const char fileName[], const argtype_t fileType);


/* read_line(FILE *)
 * =======================================================================
 * Read exactly one line from the input file passed to the program. This
 * function calls get_line() to actually read the line from the file; it
 * reads until it reaches a newline character, or the EOF character.
 *
 * Return an allocated string containing the line read from the file.
 * NOTE: If the string to hold the line cannot be allocated, it will
 *       exit the program with a memory error.
 * ======================================================================= */
char *read_line(FILE *fp);


/* write_line(FILE *, size_t, ...)
 * =======================================================================
 * Write a variable number of strings to the FILE pointer, specifically, 
 * write i strings to fp. Print a newline if the boolean variable
 * evaluates to true.
 * ======================================================================= */
void write_line(FILE *fp, bool newline, size_t i, ...);


/* write_line(FILE *, size_t, ...)
 * =======================================================================
 * Read in at most n bytes from file, or until a newline or EOF is 
 * reached. The bytes are written to s via the pointer newstr.
 *
 * Return the initialized string, or NULL if EOF was reached and no 
 * characters were read.
 * ======================================================================= */
char *get_line(char *s, int n, FILE *file);


#endif