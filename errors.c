/* errors.c
 * 
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 02/15/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file contains the implementations of functions that print dialogs 
 * for the user either of exit from the program or on request via a 
 * command-line flag argument.
 * ======================================================================= */

#include <stdio.h>

#include "errors.h"

/* global variables for use in the following functions only.
 * ======================================================================= */
const char programName[] = "patdown";
const char versionNum[]  = "0.0.3";


/* print_usage()
 * =======================================================================
 * Print the usage dialog.
 * ======================================================================= */
void print_usage(void)
{
    printf("USAGE: %s [--help] [--version] <infile> [-o <outfile>] [-f <format>]\n", programName);
}


/* print_help()
 * =======================================================================
 * Print the help dialog.
 * ======================================================================= */
void print_help(void)
{
    print_usage();
    printf("\n");
    print_version();
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


/* print_version()
 * =======================================================================
 * Print the version dialog.
 * ======================================================================= */
void print_version(void)
{
    printf("%s version %s\n", programName, versionNum);
}


/* printf_file_error()
 * =======================================================================
 * Print the file error dialog.
 * ======================================================================= */
void input_file_error(void)
{
    printf("ERROR: There was a problem reading the input file.\n");
}


/* print_memory_error()
 * =======================================================================
 * Print the memory error dialog.
 * ======================================================================= */
void print_memory_error(void)
{
    printf("ERROR: No memory available.\n");
}
