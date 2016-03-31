/* tests.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 03/27/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file contains definitions that are common to all the test_*.c
 * files.
 * ======================================================================= */

#ifndef TESTS_H
#define TESTS_H

#include <stdbool.h>


/* Tests
 * =======================================================================
 * Type to hold the various command-line flags. Each member is a Boolean
 * value that determines which tests are run.
 * ======================================================================= */
typedef struct 
{
    bool all;     // Run all tests.
    bool parsers; // Run the parsing tests.
    bool verbose; // Turn **on** the verbose function.
} Tests;


/* Colors
 * =======================================================================
 * Type to hold the ANSI color codes, so they can be called from the 
 * printf() function using the string format specifier.
 * ======================================================================= */
typedef struct 
{
    char *pass;
    char *fail;
    char *test;
    char *blue;
    char *magenta;
    char *reset;
    char *bold;
} Colors;


/* [extern] tests
 * =======================================================================
 * Global variable that will be initialize in tests.c and used throughout
 * each source file to determine which tests to run.
 * ======================================================================= */
extern Tests tests;


/* [extern] colors
 * =======================================================================
 * Global variable that will be initialize in tests.c and used throughout
 * each source file to enable colored terminal output when printing.
 * ======================================================================= */
extern Colors colors;

#endif