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
#include "errors.h"

/* Begin program execution */
int main(int argc, char const *argv[])
{
    ArgType *argt = parseArguments(argc, argv);
    printf("argc = %d\n", argc);
        
    for (size_t i = 0; i < argc; ++i)
    {
        printf("argt[%zu] = %d  ", i, argt[i]);
        printf("argv[%zu] = \'%s\'\n", i, argv[i]);
    }

    return 0;
}