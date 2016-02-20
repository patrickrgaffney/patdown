/* errors.c
 * 
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    02/15/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This file contains the implementations of the functions defined
 * in errors.h. All of these functions are used as parameters to the
 * atexit() function in stdlib.h.
 */

#include <stdio.h>

#include "errors.h"

void printUsageMsg(void)
{
    printf("USAGE: patdown [--help] [--version] <infile> [-o <outfile>] [-f <format>]\n");
}
