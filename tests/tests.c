#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "tests_parsing.h"
#include "tests.h"


/* Static functions to be called by this file only.
 * ======================================================================= */
static void parse_test_arguments(const int argc, const char *argv[]);
static void print_argc_error(void);
static void print_test_usage(void);

/* Global variable; initialized here and used elsewhere.
 * ======================================================================= */
Tests tests = {
    .all     = false,
    .parsers = false,
    .verbose = false
};

Colors colors = {
    .pass    = "\x1b[32m√",
    .fail    = "\x1b[31m✕",
    .test    = "TEST: \x1b[0m",
    .blue    = "\x1b[34m",
    .magenta = "\x1b[35m",
    .reset   = "\x1b[0m",
    .bold    = "\x1b[1m"
};


int main(int argc, const char *argv[])
{
    parse_test_arguments(argc, argv);
    
    if (tests.all)
    {
        main_parsing();
    }
    else if (tests.parsers) main_parsing();
    
    return 0;
}


/* parse_test_arguments(const int, const char *)
 * =======================================================================
 * Parse each of the command-line arguments, initializing the 
 * corresponding Boolean Tests member.
 *
 * Return the initialized Tests structure.
 * ======================================================================= */
void parse_test_arguments(const int argc, const char *argv[])
{
    if (argc < 2)
    {
        atexit(print_test_usage);
        atexit(print_argc_error);
        exit(EXIT_FAILURE);
    }
    
    for (size_t i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-v") == 0) tests.verbose = true;
        else if (strcmp(argv[i], "all") == 0) tests.all = true;
        else if (strcmp(argv[i], "parsers") == 0) tests.parsers = true;
        else 
        {
            printf("%sERROR:%s \'%s\' is not a valid argument.\n", colors.bold, colors.reset, argv[i]);
            atexit(print_test_usage);
            exit(EXIT_FAILURE);
        }
    }
}


/* print_argc_error()
 * =======================================================================
 * Print an error describing that too few arguments were passed to the 
 * program at execution.
 * ======================================================================= */
static void print_argc_error(void)
{
    printf("%sERROR:%s No arguments were supplied.\n", colors.bold, colors.reset);
}


/* print_test_usage()
 * =======================================================================
 * Print the help dialog for this program.
 * ======================================================================= */
static void print_test_usage(void)
{
    printf("USAGE: tests <options> [-v]\n\n");
    printf("  <options>    Determine which tests to run.\n");
    printf("  -v           Turn on verbose printing.\n\n");
    printf("Available options: parsers, etc.\n");
}