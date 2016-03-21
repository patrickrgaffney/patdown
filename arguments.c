/* arguments.c
 *
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    02/15/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This file parses the arguments passed to the program at execution.
 * The purpose of each individual argument is determined, and an
 * array of their determined types is sent back to the calling 
 * funtion, main().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "errors.h"
#include "arguments.h"


/* parse_arguments(const int, const char **)
 * =======================================================================
 * Build an array of argtype_t's, such that the argument argv[i] has an
 * argtype_t of argt[i].
 *
 * Return a pointer to the newly initialized array of argtype_t's.
 * ======================================================================= */
argtype_t *parse_arguments(const int argc, const char *argv[])
{
    
    argtype_t *argt = malloc(sizeof(argtype_t) * argc);
    argt[0] = PROGRAM_NAME;
    
    if (argc >= 2)
    {
        for (size_t i = 1; i < argc; ++i) 
        {
            argt[i] = test_argument(argv[i], argt[i - 1]);
        }
        check_args_for_errors(argc, argv, argt);
    }
    return argt; 
}


/* test_argument(const char *, const argtype_t)
 * =======================================================================
 * Determine if an argument is a flag or nonflag.
 *
 * Return a call to check_flags() or check_non_flags() based on the 
 * determination. Each of these functions will return an argtype_t of the
 * given argument to the caller of this function.
 * ======================================================================= */
argtype_t test_argument(const char *argument, const argtype_t prevArg)
{   
    switch (argument[0])
    {
        case '-': return check_flags(argument);
        default:  return check_non_flags(prevArg, argument);
    }
}


/* check_flags(const char *)
 * =======================================================================
 * Comare the arguments we know to be a flag with each of the valid flags
 * to determine its argtype_t.
 *
 * Return the argtype_t of the given flag.
 * NOTE: This function will exit with an error message and code if the 
 *       flag is determined to be invalid.
 * ======================================================================= */
argtype_t check_flags(const char *possibleFlag)
{
    // Static so it is initialized only once.
    static Flag flag = {
        .help    = "--help",
        .version = "--version",
        .output  = "-o",
        .format  = "-f"
    };
    
    if (strcmp(possibleFlag, flag.help) == 0)         return HELP_FLAG;
    else if (strcmp(possibleFlag, flag.version) == 0) return VERSION_FLAG;
    else if (strcmp(possibleFlag, flag.output) == 0)  return OUTPUT_FILE_FLAG;
    else if (strcmp(possibleFlag, flag.format) == 0)  return OUTPUT_FORMAT_FLAG;
    else
    {
        printf("ERROR: \'%s\' is not an valid argument.\n", possibleFlag);
        atexit(print_usage);
        exit(EXIT_FAILURE);
    }
}


/* check_non_flags(const argtype_t, const char *)
 * =======================================================================
 * Make an educated guess about the argtype_t of the non-flag argument
 * based on the argument that preceded it. If the preceding argument was
 * also a non-flag, this argument defaults to INPUT_FILE_NAME.
 *
 * Return the argtype_t of the given non-flag.
 * NOTE: This function will exit if it hits the default case of the switch
 *       statement, which means that the previous argument was also an
 *       INPUT_FILE_NAME, and currently this program only processes
 *       one file at a time.
 * ======================================================================= */
argtype_t check_non_flags(const argtype_t prevArg, const char *argument)
{
    switch (prevArg)
    {
        case PROGRAM_NAME: 
        case HELP_FLAG:
        case VERSION_FLAG:
        case OUTPUT_FILE_NAME:
        case OUTPUT_FORMAT_NAME: return INPUT_FILE_NAME;
        case OUTPUT_FILE_FLAG:   return OUTPUT_FILE_NAME;
        case OUTPUT_FORMAT_FLAG: return OUTPUT_FORMAT_NAME;
        default:
            printf("ERROR: Invalid argument: \'%s\'", argument);
            atexit(print_usage);
            exit(EXIT_FAILURE);
    }
}


/* check_non_flags(const argtype_t, const char *)
 * =======================================================================
 * Tests an arguments argtype to be sure that if the preceding argument
 * was a flag, that the current argument is a non-flag. If the test fails,
 * this function will exit the program with an error message and code.
 * ======================================================================= */
void test_argtype_for_match(const argtype_t testarg, const argtype_t reqarg, const char *errstr)
{
    if (testarg != reqarg)
    {
        printf("%s\n", errstr);
        atexit(print_usage);
        exit(EXIT_FAILURE);
    }
}


/* test_format_type(const char *)
 * =======================================================================
 * Tests a OUTPUT_FORMAT_NAME argument to determine if it is a valid
 * formattype_t.
 *
 * Return the formattype_t of arg.
 * NOTE: This function will exit if the arg in question is found to be an
 *       invalid formattype_t.
 * ======================================================================= */
formattype_t test_format_type(const char *arg)
{
    // Static so it is initialized only once.
    static Format format = {
        .html = "html",
        .txt  = "txt"
    };
    size_t arglen = strlen(arg);
    formattype_t returnType;
    
    char *lowerArg = malloc(sizeof(char) * arglen + 1);
    
    // Convert possibleFormat to all lowercase
    for (size_t i = 0; i < arglen; ++i) 
    {
        lowerArg[i] = tolower(arg[i]);
    }
    lowerArg[arglen] = '\0';
    
    if (strcmp(lowerArg, format.html) == 0)     returnType = HTML_FORMAT;
    else if (strcmp(lowerArg, format.txt) == 0) returnType = TXT_FORMAT;
    else
    {
        free(lowerArg);
        printf("ERROR: \'%s\' is not an valid output format.\n", arg);
        atexit(print_usage);
        exit(EXIT_FAILURE);
    }
    
    free(lowerArg);
    return returnType;
}


/* check_args_for_errors(const int, const char *, const argtype_t *)
 * =======================================================================
 * Driver for the various test_*() functions. Determines if all the 
 * arguments provided are valid, and that each flag that requires another
 * argument has it.
 *
 * NOTE: This function will exit with a message and code if any of its
 *       tests fails. Failing these tests is fatal.
 * ======================================================================= */
void check_args_for_errors(const int argc, const char *argv[], const argtype_t *argt)
{
    size_t inputFiles  = 0;
    size_t formatTypes = 0;
    
    for (size_t i = 1; i < argc; ++i)
    {   
        if (argt[i] == OUTPUT_FILE_FLAG)
        {
            char errstr[] = "ERROR: Output file flag requires file name.";
            test_argtype_for_match(argt[i + 1], OUTPUT_FILE_NAME, errstr);
        }
        else if (argt[i] == OUTPUT_FORMAT_FLAG)
        {
            char errstr[] = "ERROR: Output format flag requires format name.";
            test_argtype_for_match(argt[i + 1], OUTPUT_FORMAT_NAME, errstr);
        }
        else if (argt[i] == INPUT_FILE_NAME)
        {
            if (++inputFiles > 1)
            {
                printf("ERROR: Multiple input files were provided.\n");
                atexit(print_usage);
                exit(EXIT_FAILURE);
            }
        }
        else if (argt[i] == OUTPUT_FORMAT_NAME)
        {
            if (++formatTypes > 1)
            {
                printf("ERROR: Multiple format types were provided.\n");
                atexit(print_usage);
                exit(EXIT_FAILURE);
            }
            test_format_type(argv[i]);
        }
    }
}