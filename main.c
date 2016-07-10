/* 
 * main.c -- parse arguments and begin program execution
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 06/22/2016.
 * 
 *********ultrapatbeams*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "arguments.h"
#include "markdown.h"
#include "parsers.h"
#include "files.h"
#include "errors.h"


/******************************************************************
 * alloc_argt() -- allocate space for an array of arg_t elements
 *
 * const int size -- the number of elements needed for the array
 *
 * @throws -- throw_memory_error() if space cannot be allocated
 * @return -- a newly allocated arg_t array
 ******************************************************************/
static arg_t *alloc_argt(const int size)
{
    arg_t *type = NULL;
    type = malloc(sizeof(arg_t) * size);
    if (!type) throw_memory_error();
    return type;
}


/******************************************************************
 * free_argt() -- free the memory allocated for the arg_t array
 *
 * arg_t *types -- the array to be freed
 *
 * @noreturn -- control returns to the caller
 ******************************************************************/
static void free_argt(arg_t *types)
{
    free(types);
}


/******************************************************************
 * check_output_type() -- determine if user output type is valid
 *
 * const char *arg -- the user provided output type
 *
 * @throws -- throw_invalid_output_t_error() if type is invalid
 * @return -- the determined arg_t output constant
 ******************************************************************/
static arg_t check_output_type(const char *arg)
{
    const size_t size = strlen(arg);
    char *lowerArg = malloc(sizeof(char) * (size + 1));
    while (*arg != '\0') *lowerArg++ = tolower(*arg++);
    *lowerArg = '\0';
    lowerArg  = lowerArg - size;
    
    if (strcmp(lowerArg, "html") != 0) {
        throw_invalid_output_t_error(arg - size);
    }
    free(lowerArg);
    return OUTPUT_TYPE_HTML;
}


/******************************************************************
 * parse_flag_option() -- parse a command-line flag option
 *
 * const char *arg -- the user provided flag option
 *
 * @throws -- throw_invalid_option_error() if flag is invalid
 * @return -- the determined arg_t flag constant
 ******************************************************************/
static arg_t parse_flag_option(const char *arg)
{
    if (*(++arg) == '\0') {
        throw_invalid_option_error(arg);
    }
    else if (*arg == 'o') {
        return OUTPUT_FILE_FLAG;
    }
    else if (*arg == 't') {
        return OUTPUT_TYPE_FLAG;
    }
    else throw_invalid_option_error(arg);
    
    return INPUT_FILE_NAME;
}


/******************************************************************
 * parse_string_option() -- parse a command-line string option
 *
 * const char *arg  -- the user provided string option
 * const arg_t last -- the last parsed command-line argument
 *
 * @return -- the determined arg_t constant
 ******************************************************************/
static arg_t parse_string_option(const char *arg, const arg_t last)
{
    switch (last) {
        case OUTPUT_FILE_FLAG:
            return OUTPUT_FILE_NAME;
        case OUTPUT_TYPE_FLAG:
            return check_output_type(arg);
        default: 
            return INPUT_FILE_NAME;
    }
}


/******************************************************************
 * parse_arguments() -- begin parsing command-line arguments
 *
 * const int argc    -- the number of command-line arguments
 * const char **argv -- a string array of the arguments
 *
 * @return -- an array containing each arguments type (arg_t)
 ******************************************************************/
static arg_t *parse_arguments(const int argc, const char **argv)
{
    arg_t *types = alloc_argt(argc);
    types[0] = PROGRAM_NAME;
    arg_t lastType = PROGRAM_NAME;
    
    for (size_t i = 1; i < argc; i++) {
        if (*argv[i] == '-') {
            types[i] = parse_flag_option(argv[i]);
        }
        else {
            types[i] = parse_string_option(argv[i], lastType);
        }
        lastType = types[i];
    }
    return types;
}


/******************************************************************
 * main() -- parse arguments and begin program execution
 *
 * const int argc    -- the number of command-line arguments
 * const char **argv -- a string array of the arguments
 *
 * @return -- EXIT_SUCCESS
 ******************************************************************/
int main(int argc, char const **argv)
{
    options_t opts = {
        .inFileName  = NULL,
        .outFileName = NULL,
        .outputType  = HTML_OUT,
        .inputFile   = stdin,
        .outputFile  = stdout
    };
    markdown_t *queue = NULL;
    arg_t *type = parse_arguments(argc, argv);
    
    for (size_t i = 1; i < argc; i++) {
        // printf("argt = %d, argv = \'%s\'\n", type[i], argv[i]);
        switch (type[i]) {
            case INPUT_FILE_NAME: 
                opts.inFileName = argv[i];
                break;
            case OUTPUT_FILE_NAME:
                opts.outFileName = argv[i];
                break;            
            case OUTPUT_TYPE_HTML:
                opts.outputType = HTML_OUT;
                break;
            default: break;
        }
    }
    
    if (opts.inFileName) {
        opts.inputFile = open_file(opts.inFileName);
    }
    
    // if output file -- open that file for writing
    
    queue = markdown(opts.inputFile);
    print_markdown_queue(queue);
   
    free_argt(type);
    free_markdown(queue);
    return EXIT_SUCCESS;
}