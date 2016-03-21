/* main.c
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 02/15/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 * 
 * =======================================================================
 * This file serves as the entry to the program. It starts by parsing the 
 * arguments, and they determine the function of the program from there.
 * =======================================================================
 *   patdown [--help] [--version] <infile> [-o <outfile>] [-f <format>]
 * ======================================================================= */

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
    formattype_t outputFormat = HTML_FORMAT;
    argtype_t *argt;
    MarkdownBlock *headPtr = NULL;
    
    if (argc > 1)
    {
        argt = parse_arguments(argc, argv);
        
        for (size_t i = 0; i < argc; ++i)
        {
            switch (argt[i])
            {
                case INPUT_FILE_NAME:    inputFile = openFile(argv[i], argt[i]); break;
                case OUTPUT_FILE_NAME:   outputFile = openFile(argv[i], argt[i]); break;
                case OUTPUT_FORMAT_NAME: outputFormat = test_format_type(argv[i]); break;
                case HELP_FLAG:          print_help(); break;
                case VERSION_FLAG:       print_version(); break;
                default: continue;
            }
        }
    }
    
    // Consume all lines from inputFile
    headPtr = build_queue(inputFile);
    
    // Process them using outputFormat
    
    // Write all lines to outputFile
    print_queue(headPtr, outputFile);
    
    if (argt) free(argt);
    free_block(headPtr);
    
    return 0;
}
