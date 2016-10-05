/***** 
 * errors.c -- methods for throwing fatal and non-fatal errors
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-10-04
 * 
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "errors.h"

/************************************************************************
 * Fatal Errors
 *
 *  All of these errors cause program execution to be halted.
 ************************************************************************/

/*****
 * Fatal error: memory could not be allocated. Exit with dialog.
 *****/
void throw_fatal_memory_error(void)
{
    fprintf(stderr, "FATAL: memory could not be allocated.\n");
    exit(EXIT_FAILURE);
}

/*****
 * Fatal error: file could not be opened. Exit with dialog.
 *
 * ARGUMENTS
 *  file    The name of the file that could not be opened.
 *****/
void throw_fatal_fopen_error(const char *file)
{
    fprintf(stderr, "FATAL: file could not be opened: \'%s\'\n", file);
    exit(EXIT_FAILURE);
}


/************************************************************************
 * Non-Fatal Errors
 *
 *  None of these errors cause program execution to be halted. Instead, 
 *  they print out an error dialog and return control to the caller.
 ************************************************************************/
