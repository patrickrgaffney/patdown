/* 
 * errors.c -- methods for throwing errors.
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 06/15/2016.
 * 
 *********ultrapatbeams*/

#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

/******************************************************************
 * throw_memory_error() -- memory could not be allocated
 *
 * @fatal -- exits program with error dialog
 ******************************************************************/
void throw_memory_error(void)
{
    fprintf(stderr, "FATAL: memory could not be allocated.\n");
    exit(EXIT_FAILURE);
}


/******************************************************************
 * throw_file_opening_error() -- file could not be opened
 * 
 * const char *filename -- the name of the unopened file
 *
 * @fatal -- exits program with error dialog
 ******************************************************************/
void throw_file_opening_error(const char *fileName)
{
    fprintf(stderr, "FATAL: \'%s\' could not be opened.\n", fileName);
    exit(EXIT_FAILURE);
}


/******************************************************************
 * throw_invalid_option_error() -- provided option flag is invalid
 *
 * const char *argument -- the invalid option flag
 *
 * @fatal -- exits program with error dialog
 ******************************************************************/
void throw_invalid_option_error(const char *argument)
{
    fprintf(stderr, "ERROR: \'%s\' is an invalid option.\n", argument);
    exit(EXIT_FAILURE);
}


/******************************************************************
 * throw_invalid_output_t_error() -- output type is invalid
 *
 * const char *argument -- the invalid output type
 *
 * @fatal -- exits program with error dialog
 ******************************************************************/
void throw_invalid_output_t_error(const char *argument)
{
    fprintf(stderr, "ERROR: \'%s\' is an invalid output type.\n", argument);
    exit(EXIT_FAILURE);
}


/******************************************************************
 * throw_multiple_input_files_error() -- multiple input files 
 *                                       provided
 *
 * @fatal -- exits program with error dialog
 ******************************************************************/
void throw_multiple_input_files_error(void)
{
    fprintf(stderr, "FATAL: Multiple input files were provided.\n");
    exit(EXIT_FAILURE);
}