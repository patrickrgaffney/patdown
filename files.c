/***** 
 * files.c -- opening, reading, and writing to files
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-17
 * @modified    2016-09-30
 * 
 ************************************************************************/

#include <stdio.h>

#include "errors.h"
#include "files.h"
#include "strings.h"


/** Open a file stream for reading. *************************************/
FILE *open_file(const char *fileName)
{
    FILE *filePtr = NULL;
    filePtr = fopen(fileName, "r");
    if (!filePtr) throw_fatal_fopen_error(fileName);
    return filePtr;
}


/** Close a file stream. ************************************************/
void close_file(FILE *io)
{
    if (io) fclose(io);
}


/** Read a line of text from inputFile. *********************************/
String *read_line(FILE *inputFile)
{
    // if (feof(inputFile)) return NULL;
    
    int c = 0;          /* Character read from inputFile. */
    int i = 0;          /* Index to iterate over the new string. */
    int lim = 2500;     /* Max number of characters to allocate for. */
    int order = 1;      /* Multiplier for lim if we realloc() string. */
    
    String *newstr = init_string(lim);
    
    while ((c = getc(inputFile)) != EOF && c != '\n') {
        
        /* Reallocate string if we have read lim characters from 
         * inputFile and havent reached a newline yet. */
        if (--lim == 0) {
            lim = newstr->size * ++order;
            newstr->size = lim;
            newstr->string = realloc_char_array(newstr->string, newstr->size);
        }
        
        /* Convert all tabs to 4 spaces -- makes for easier parsing. */
        if (c == '\t') {
            for (size_t j = 0; j < 4; j++) newstr->string[i++] = ' ';
            lim -= 4;
        }
        else newstr->string[i++] = c;
    }
    if (c == EOF && i == 0) {
        printf("EOF ENCOUNTERED\n");
        free_string(newstr);
        return NULL;
    }
    newstr->len = i;
    newstr->string[i] = '\0';
    return newstr;
}