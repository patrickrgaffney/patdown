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
 * This file contains the implementations of functions that open and 
 * operate on various types of files (input and output) for processing.
 * ======================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "files.h"
#include "arguments.h"
#include "errors.h"


/* open_file(const char *, const argtype_t)
 * =======================================================================
 * Open a file for reading (type "r") or writing (type "w") based on the
 * argtype_t passed to the funtion.
 *
 * Return a pointer to the opened file.
 * NOTE: If the file cannot be opened, then the program will exit with an
 *       error message and code.
 * ======================================================================= */
FILE *open_file(const char fileName[], const argtype_t fileType)
{
    FILE *filePtr = NULL;
    
    if (fileType == INPUT_FILE_NAME)
    {
        filePtr = fopen(fileName, "r");
        
        if (filePtr == NULL) printf("ERROR: Input file ");
        else return filePtr;
    }
    else if (fileType == OUTPUT_FILE_NAME)
    {
        filePtr = fopen(fileName, "w");
        
        if (filePtr == NULL) printf("ERROR: Output file ");
        else return filePtr;
    }
    printf("\'%s\' could not be opened.\n", fileName);
    atexit(print_usage);
    exit(EXIT_FAILURE);
}


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
char *read_line(FILE *fp)
{
    const size_t MAX_LINE = 5000;
    char *line =(char *) malloc(sizeof(char) * MAX_LINE);
    
    if (line != NULL)
    {
        if (get_line(line, MAX_LINE, fp) != NULL) return line;
        else { return NULL; }
    }
    else
    {
        atexit(print_memory_error);
        exit(EXIT_FAILURE);
    }
}


/* write_line(FILE *, size_t, ...)
 * =======================================================================
 * Write a variable number of strings to the FILE pointer, specifically, 
 * write i strings to fp. Print a newline at after printing all strings.
 * ======================================================================= */
void write_line(FILE *fp, size_t i, ...)
{
    va_list ap;
    va_start(ap, i);
    size_t j = 0;
    
    while (j++ < i) fprintf(fp, "%s", va_arg(ap, char *));
    
    va_end(ap);
    printf("\n");
}


/* write_line(FILE *, size_t, ...)
 * =======================================================================
 * Read in at most n bytes from file, or until a newline or EOF is 
 * reached. The bytes are written to s via the pointer newstr.
 *
 * Return the initialized string, or NULL if EOF was reached and no 
 * characters were read.
 * ======================================================================= */
char *get_line(char *s, int n, FILE *file)
{
    char *newstr = s; // Pointer to be incremented.
    int c;            // Character to be read
    
    while (--n > 0 && (c = fgetc(file)))
    {
        if (c == '\n' || c == EOF) break;
        else { *newstr++ = c; }
    }
    *newstr = '\0';
    
    if (c == EOF && s == newstr) { free(s); return NULL; }
    else return newstr;
}