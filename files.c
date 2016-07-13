/* 
 * files.c -- opening, reading, and writing to files
 * 
 * Created by PAT GAFFNEY on 06/17/2016.
 * Last modified on 07/12/2016.
 * 
 *********ultrapatbeams*/

#include <stdio.h>
#include "files.h"
#include "errors.h"
#include "strings.h"


/******************************************************************
 * open_file() -- open a file stream
 *
 * const char *fileName -- name of file to be opened
 *
 * @throws -- throw_file_opening_error() if file cannot be opened
 * @return -- pointer to an open file stream
 ******************************************************************/
FILE *open_file(const char *fileName)
{
    FILE *filePtr = NULL;
    filePtr = fopen(fileName, "r");
    if (!filePtr) throw_file_opening_error(fileName);
    return filePtr;
}


/******************************************************************
 * close_file() -- close a file stream
 *
 * FILE *stream -- file stream to be closed
 *
 * @noreturn -- control returned to the caller
 ******************************************************************/
void close_file(FILE *io)
{
    if (io) fclose(io);
}


/******************************************************************
 * read_line() -- read from inputFile until a newline is reached
 *
 * FILE *inputFile -- file from which characters are read
 *
 * @return -- readstring_t structure containing string from file
 ******************************************************************/
string_t *read_line(FILE *inputFile)
{
    int c = 0, i = 0, lim = 2500, order = 1;
    string_t *newstr = init_stringt(lim);
    
    while ((c = getc(inputFile)) != EOF && c != '\n') {
        if (--lim == 0) {
            lim = newstr->size * ++order;
            newstr->size = lim;
            newstr->string = realloc_string(newstr->string, newstr->size);
        }
        
        // Convert all tabs to 4 spaces -- makes for easier parsing.
        if (c == '\t') {
            for (size_t j = 0; j < 4; j++) newstr->string[i++] = ' ';
            lim -= 4;
        }
        else newstr->string[i++] = c;
    }
    newstr->len = i;
    printf("read %d chars\n", i);
    newstr->string[i] = '\0';
    return newstr;
}