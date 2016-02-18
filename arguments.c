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

#include "errors.h"
#include "arguments.h"


/* If arguments were passed to the program at execution, parse them
 * to determine the programs purpose. Return an array of size `argc`
 * where each element contains the ArgType of the corresponding 
 * argument, i.e:
 *
 *  argt[i] is the ArgType of argv[i]
 */
ArgType *parseArguments(const int argc, const char *argv[])
{
    // If there are < 2 arguments, stdin will be the input file
    const unsigned int reqNumArgsForFile = 2;
    
    // Allocate memory for array of ArgTypes
    ArgType *argt = malloc(sizeof(ArgType) * argc);
    
    // The first argument is always program name
    argt[0] = PROGRAM_NAME;
    
    if (argc >= reqNumArgsForFile)
    {
        for (size_t i = 1; i < argc; ++i)
        {
            argt[i] = testProgramArgument(argv[i], argt[i - 1]);
        }
    }
    
    /* TODO: Loop through argt[] :: checking for weird shit
        - multiple input names
        - provided a flag that requires a name, but no name
    */
    
    return argt;
}


/* Test a string against the variety of possible command-line
 * arguments, return the purpose of that specific argument.
 */
ArgType testProgramArgument(const char argument[], const ArgType prevArg)
{   
    switch (argument[0])
    {
        case '-': // Check for flags
            return checkFlags(argument);
        
        default: // Defaults to input file
            return checkNonFlags(prevArg, argument);
    }
}


/* Compare the input string with the different possible command-
 * line arguments (flags) and return the ArgType of that flag. Exit
 * the program if the flag is not recognizable.
 */
ArgType checkFlags(const char possibleFlag[])
{
    // Static declaration of valid flags
    static Flag flag;
    flag.help    = "--help";
    flag.version = "--version";
    flag.output  = "-o";
    flag.format  = "-f";
    
    if (strcmp(possibleFlag, flag.help) == 0)
    {
        return HELP_FLAG;
    }
    else if (strcmp(possibleFlag, flag.version) == 0)
    {
        return VERSION_FLAG;
    }
    else if (strcmp(possibleFlag, flag.output) == 0)
    {
        return OUTPUT_FILE_FLAG;
    }
    else if (strcmp(possibleFlag, flag.format) == 0)
    {
        return OUTPUT_FORMAT_FLAG;
    }
    else
    {
        printf("\'%s\' is not an valid argument.\n", possibleFlag);
        atexit(printUsageMsg);
        exit(EXIT_FAILURE);
    }
}


/* Makes an educated guess about the ArgType of the command-line
 * argument based on the argument that preceeded it.
 */
ArgType checkNonFlags(const ArgType prevArg, const char argument[])
{
    switch (prevArg)
    {
        // Defaults to input file
        case PROGRAM_NAME: 
        case HELP_FLAG:
        case VERSION_FLAG:
            return INPUT_FILE_NAME;
        
        case OUTPUT_FILE_FLAG:
            return OUTPUT_FILE_NAME;
        
        case OUTPUT_FORMAT_FLAG:
            return OUTPUT_FORMAT_NAME;
        
        default:
            printf("Invalid argument: %s", argument);
            atexit(printUsageMsg);
            exit(EXIT_FAILURE);
    }
}