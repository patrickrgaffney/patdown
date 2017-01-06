/**
 * errors.c -- methods for throwing fatal and non-fatal errors
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2017-01-05
 *  project:    patdown
 * 
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "errors.h"

/** Fatal error: memory could not be allocated. Exit with dialog. */
void throw_fatal_memory_error(void)
{
    fprintf(stderr, "FATAL: memory could not be allocated.\n");
    exit(EXIT_FAILURE);
}
