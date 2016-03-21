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

/* Open a file for either reading or writing data. If the file cannot
 * be opened, exit with an error dialog. Otherwise, return the file
 * pointer to the calling function.
 */
FILE *openFile(const char fileName[], const argtype_t fileType)
{
    FILE *filePtr = NULL;
    
    if (fileType == INPUT_FILE_NAME)
    {
        // Open file for READING
        filePtr = fopen(fileName, "r");
        
        if (filePtr == NULL) { printf("ERROR: Input file "); }
        else { return filePtr; }
    }
    else if (fileType == OUTPUT_FILE_NAME)
    {
        // Open file for WRITING
        filePtr = fopen(fileName, "w");
        
        if (filePtr == NULL) { printf("ERROR: Output file "); }
        else { return filePtr; }
    }
    
    printf("\'%s\' could not be opened.\n", fileName);
    atexit(print_usage);
    exit(EXIT_FAILURE);
}


/* Read exactly one line (until next newline) from the file pointed
 * to by *fp.
 */
char *readLine(FILE *fp)
{
    const int MAX_LINE = 1208;
    char *line =(char *) malloc(sizeof(char) * MAX_LINE);
    
    if (line != NULL)
    {
        if (fgetline(line, MAX_LINE, fp) != NULL)
        {
            size_t ln = strlen(line) - 1;
            if (*line && line[ln] == '\n') 
                line[ln] = '\0';
            
            return line;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        atexit(print_memory_error);
        exit(EXIT_FAILURE);
    }
}


/* Write `i` string's received as input to the file pointed to by 
 * `*fp`. Exit the function by printing a newline `\n`. 
 */
void writeLine(FILE *fp, size_t i, ...)
{
    // stores information needed by va_start and va_end
    va_list ap;
    
    // initialize the va_list object
    va_start(ap, i);
    
    for (size_t j = 0; j < i; ++j)
    {
        fprintf(fp, "%s", va_arg(ap, char *));
    }
    
    va_end(ap);
    printf("\n");
}


/* This is basically a rewrite of the fgets() function in clib, the
 * only difference being that it does NOT add the `\n` newline to the
 * string it reads from the file.
 */
char *fgetline(char *s, int n, FILE *file)
{
    char *newstr = s;
    int c;
    int num = 0;
    
    while ((c = fgetc(file)))
    {
        if (c == '\n' || c == EOF) { break; }
        else {
            *newstr++ = c;
            num++;
        }
    }
    *newstr = '\0';
    
    if (num == 0 && c == EOF)
    {
        free(s);
        return NULL;
    }
    // else if (num == 0)
    // {
    //     return newstr;
    // }
    else
    {
        return newstr;
    }
}