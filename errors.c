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
 * in errors.h. All of these functions are _mainly_ used as 
 * parameters to the atexit() function in stdlib.h.
 */

#include <stdio.h>

#include "errors.h"

/* Global variables */
const char programName[] = "patdown";
const char versionNum[]  = "0.0.1";


/* Display USAGE dialog */
void printUsageMsg(void)
{
    printf("USAGE: %s [--help] [--version] <infile> [-o <outfile>] [-f <format>]\n",
        programName);
}

/* Display HELP dialog */
void printHelpMsg(void)
{
    printUsageMsg();
    printf("\n");
    printVersionMsg();
    printf("\n");
    printf("  Stand-alone options:\n");
    printf("  --verison      Print program version\n");
    printf("  --help         Print program help dialog\n");
    printf("\n");
    printf("  Options that require additional arguments:\n");
    printf("  -o <outfile>   Send the output to <outfile>\n");
    printf("                 Default is <stdout>\n");
    printf("  -f <format>    Convert file to <format>\n");
    printf("                 Default is <html>\n");
    printf("\n");
    printf("Available formats: html, txt\n");
}

/* Display VERSION dialog */
void printVersionMsg(void)
{
    printf("%s version %s\n", programName, versionNum);
}

/* Display INPUT FILE READ error */
void inputFileError(void)
{
    printf("There was a problem reading the input file.\n");
}

/* Display MEMORY ERROR exit message. */
void printMemoryError(void)
{
    printf("No memory available.\n");
}