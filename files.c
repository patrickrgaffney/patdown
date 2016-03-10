/* files.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    02/15/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This file contains the implementations of functions that open 
 * various types of files (INPUT, OUPTUT) for processing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "files.h"
#include "arguments.h"
#include "errors.h"

/* Open a file for either reading or writing data. If the file cannot
 * be opened, exit with an error dialog. Otherwise, return the file
 * pointer to the calling function.
 */
FILE *openFile(const char fileName[], const ArgType fileType)
{
    FILE *filePtr = NULL;
    
    if (fileType == INPUT_FILE_NAME)
    {
        // Open file for READING
        filePtr = fopen(fileName, "r");
        
        if (filePtr == NULL)
        {
            printf("ERROR: Input file ");
        }
        else
        {
            return filePtr;
        }
    }
    else if (fileType == OUTPUT_FILE_NAME)
    {
        // Open file for WRITING
        filePtr = fopen(fileName, "w");
        
        if (filePtr == NULL)
        {
            printf("ERROR: Output file ");
        }
        else
        {
            return filePtr;
        }
    }
    
    printf("\'%s\' could not be opened.\n", fileName);
    atexit(printUsageMsg);
    exit(EXIT_FAILURE);
}


/* Read exactly one line (until next newline) from the file pointed
 * to by *fp.
 */
char *readLine(FILE *fp)
{
    const int MAX_LINE = 6400;
    char *line = malloc(sizeof(char) * MAX_LINE);
    
    if (line != NULL)
    {
        if (fgetline(line, MAX_LINE, fp) != NULL)
        {
            return line;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        atexit(printMemoryError);
        exit(EXIT_FAILURE);
    }
}


/* Wrtie exactly one line (received as input) to the file pointed to
 * by *fp.
 */
int writeLine(const char *line, FILE *fp)
{
    if (fputs(line, fp) != EOF)
    {
        return 1;
    }
    else
    {
        return 0;
    }
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
    
    while (--n > 0)
    {
        c = getc(file);
        if (c == '\n' || c == EOF) { break; }
        else {
            *newstr++ = c;
            num++;
        }
    }
    *newstr = '\0';
    
    if (num == 0 && c == EOF)
    {
        return NULL;
    }
    else if (num == 0)
    {
        return newstr;
    }
    else
    {
        return newstr;
    }
}