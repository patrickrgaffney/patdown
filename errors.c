/***** 
 * errors.c -- methods for throwing fatal and non-fatal errors
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-10-01
 * 
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "errors.h"

/************************************************************************
 * @section Fatal Errors
 *
 *  All of these errors cause program execution to be halted.
 ************************************************************************/

/** FATAL-ERROR: memory could not be allocated **************************/
void throw_memory_error(void)
{
    fprintf(stderr, "FATAL: memory could not be allocated.\n");
    exit(EXIT_FAILURE);
}

/** FATAL-ERROR: file could not be opened *******************************/
void throw_file_opening_error(const char *file)
{
    fprintf(stderr, "FATAL: file could not be opened: \'%s\'\n", file);
    exit(EXIT_FAILURE);
}

/** FATAL-ERROR: multiple input files provided **************************/
void throw_multiple_input_files_error(void)
{
    fprintf(stderr, "FATAL: Multiple input files provided.\n");
    exit(EXIT_FAILURE);
}


/************************************************************************
 * @section Non-Fatal Errors
 *
 *  None of these errors cause program execution to be halted. Instead, 
 *  they print out an error dialog and return control to the caller.
 ************************************************************************/

/** NON-FATAL-ERROR: unknown output type. *******************************/
void throw_invalid_output_type(const char *type)
{
    fprintf(stderr, 
            "WARNING: unknown output type: \'%s\' -- defaulting to HTML\n", 
            type);
}