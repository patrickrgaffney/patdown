/***** 
 * main.c -- parse arguments and begin program execution
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-08-28
 * 
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "arguments.h"
#include "markdown.h"
#include "parsers.h"
#include "files.h"
#include "errors.h"


/******************************************************************************
 * @section Command-line Argument Parsing
 * 
 * First, all command line arguments are given an `arg_t`, or *argument type*.
 * These different types (`INPUT_FILE_NAME`, `OUTPUT_TYPE_HTML`, etc) change
 * the various program options from their defaults.
 *****************************************************************************/

/*****
 * Allocate memory for an array of `arg_t` elements.
 *
 * @param   size    The number of elements (arguments) in the array.
 *
 * @throws  throw_memory_error()
 * @return  A newly allocated `arg_t` array.
 *****************************************************************************/
static arg_t *alloc_argt(const int size)
{
    arg_t *type = NULL;
    type = malloc(sizeof(arg_t) * size);
    if (!type) throw_memory_error();
    return type;
}


/*****
 * Free the memory allocated for the `arg_t` array.
 *
 * @param   types   The array to be free'd.
 *****************************************************************************/
static void free_argt(arg_t *types)
{
    free(types);
}


/*****
 * Determine if the user's output type is valid.
 *
 * @param   arg     The user provided output type.
 * 
 * @throws  throw_invalid_output_t_error()
 * @return  The determined `arg_t` output constant.
 *****************************************************************************/
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


/*****
 * Parse a command-line flag option.
 *
 * @param   arg     The user provided flag option.
 * 
 * @throws  throw_invalid_option_error()
 * @return  The determined `arg_t` flag constant.
 *****************************************************************************/
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


/*****
 * Parse a command-line string option.
 *
 * Any string options whose type (`arg_t`) cannot be determine default to being
 * `INPUT_FILE_NAME` type, and are therefore attempted to be opened as a file.
 *
 * @param   arg     The user provided string option.
 * @param   last    The type of the last parsed markdown block.
 *
 * @return  The determined `arg_t` constant.
 *****************************************************************************/
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


/*****
 * Being parsing the command-line arguments.
 *
 * @param   argc    The number of command-line arguments.
 * @param   argv    A string array of the arguments.
 *
 * @return  An array containing each argument's type, or `arg_t`.
 *****************************************************************************/
static arg_t *parse_arguments(const int argc, const char **argv)
{
    arg_t *types = alloc_argt(argc);
    types[0] = PROGRAM_NAME;
    arg_t lastType = PROGRAM_NAME;
    
    for (size_t i = 1; i < argc; i++) {
        if (*argv[i] == '-') {
            types[i] = parse_flag_option(argv[i]);
        }
        else types[i] = parse_string_option(argv[i], lastType);
        lastType = types[i];
    }
    return types;
}


/******************************************************************************
 * @section Progam Execution
 *****************************************************************************/

/*****
 * Begin program execution.
 *
 * @param   argc    The number of command-line arguments.
 * @param   argv    A string array of the arguments.
 *
 * @return  `EXIT_SUCCESS`.
 *****************************************************************************/
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
    close_file(opts.inputFile);
    free_argt(type);
    free_markdown(queue);
    return EXIT_SUCCESS;
}