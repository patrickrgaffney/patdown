/**
 * errors.c -- methods for throwing errors
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-15
 * 
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "errors.h"

/**
 * throw_memory_error() -- memory could not be allocated
 **
 * @fatal -- exits program with error dialog
 ************************************************************************/
void throw_memory_error(void)
{
    fprintf(stderr, "FATAL: memory could not be allocated.\n");
    exit(EXIT_FAILURE);
}


/**
 * throw_file_opening_error(file) -- file could not be opened
 **
 * @fatal -- exits program with error dialog
 ************************************************************************/
void throw_file_opening_error(const char *file)
{
    fprintf(stderr, "FATAL: \'%s\' could not be opened.\n", file);
    exit(EXIT_FAILURE);
}


/**
 * throw_multiple_input_files_error() -- multiple input files provided
 **
 * @fatal -- exits program with error dialog
 ************************************************************************/
void throw_multiple_input_files_error(void)
{
    fprintf(stderr, "FATAL: Multiple input files were provided.\n");
    exit(EXIT_FAILURE);
}


// /*
//  * throw_invalid_option_error() -- provided option flag is invalid
//  *
//  * const char *argument -- the invalid option flag
//  *
//  * @fatal -- exits program with error dialog
//  *****************************************************************/
// void throw_invalid_option_error(const char *arg)
// {
//     fprintf(stderr, "ERROR: \'%s\' is an invalid option.\n", argument);
//     exit(EXIT_FAILURE);
// }
//
//
// /******************************************************************
//  * throw_invalid_output_t_error() -- output type is invalid
//  *
//  * const char *argument -- the invalid output type
//  *
//  * @fatal -- exits program with error dialog
//  ******************************************************************/
// void throw_invalid_output_t_error(const char *argument)
// {
//     fprintf(stderr, "ERROR: \'%s\' is an invalid output type.\n", argument);
//     exit(EXIT_FAILURE);
// }