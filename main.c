/* main.c
 *
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    02/15/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 * 
 * This file serves as the entry into the program. First, the 
 * command-line arguments are parsed, or if none were supplied, data
 * is read from `stdin`. Then, the appropriate functions are called.
 * 
 * patdown [--help] [--version] <infile> [-o <outfile>] [-f <format>]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arguments.h"
#include "files.h"
#include "errors.h"
#include "markdown.h"

int main(int argc, char const *argv[])
{
    FILE *inputFile  = stdin;
    FILE *outputFile = stdout;
    FormatType outputFormat = HTML_FORMAT;
    ArgType *argt;
    blockNode *headPtr = NULL;
    
    // If arguments were provided, determine their purpose
    if (argc > 1)
    {
        argt = parseArguments(argc, argv);
        
        for (size_t i = 0; i < argc; ++i)
        {
            if (argt[i] == INPUT_FILE_NAME)
            {
                inputFile = openFile(argv[i], argt[i]);
            }
            else if (argt[i] == OUTPUT_FILE_NAME)
            {
                outputFile = openFile(argv[i], argt[i]);
            }
            else if (argt[i] == OUTPUT_FORMAT_NAME)
            {
                outputFormat = testFormatType(argv[i]);
            }
            else if (argt[i] == HELP_FLAG)
            {
                printHelpMsg();
            }
            else if (argt[i] == VERSION_FLAG)
            {
                printVersionMsg();
            }
        }
    }
    
    // Consume all lines from inputFile
    headPtr = buildList(inputFile);
    
    // Process them using outputFormat
    
    // Write all lines to outputFile
    printQueue(headPtr, outputFile);
    
    return 0;
}
