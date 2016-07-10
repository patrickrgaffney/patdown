/* 
 * arguments.h -- methods for argument parsing
 * 
 * Created by PAT GAFFNEY on 06/21/2016.
 * Last modified on 06/21/2016.
 * 
 *********ultrapatbeams*/

#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdio.h>


/******************************************************************
 * arg_t() -- type representing valid command-line arg constants
 ******************************************************************/
typedef enum {
    PROGRAM_NAME,
    INPUT_FILE_NAME,
    OUTPUT_FILE_FLAG,
    OUTPUT_FILE_NAME,
    OUTPUT_TYPE_FLAG,
    OUTPUT_TYPE_HTML
} arg_t;


/******************************************************************
 * output_t() -- type representing valid output type constants
 ******************************************************************/
typedef enum {
    HTML_OUT
} output_t;


/******************************************************************
 * options_t() -- object containing various options for the program
 *
 * const char *inFileName  -- name of input file (default: stdin)
 * const char *outFileName -- name of output file (default: stdout)
 * output_t outputType     -- type of output file (default: HTML)
 * FILE *inputFile         -- pointer to the input file
 * FILE *outputFile        -- pointer to the output file
 ******************************************************************/
typedef struct {
    const char *inFileName;
    const char *outFileName;
    output_t outputType;
    FILE *inputFile;
    FILE *outputFile;
} options_t;


#endif