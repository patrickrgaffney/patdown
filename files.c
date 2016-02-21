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