/***** 
 * main.c -- parse arguments and open files
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-15
 * 
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>

#include "errors.h"
#include "files.h"
#include "markdown.h"
#include "parsers.h"


/************************************************************************
 * @section Command-line Argument Parsing
 ************************************************************************/

/**
 * output_t -- type representing valid output type constants
 ************************************************************************/
typedef enum 
{
    HTML_OUT    /* Default -- HTML5 element syntax. */
} output_t;


/**
 * options_t -- container for various program options
 ************************************************************************/
struct progopts 
{
    const char *inFileName;     /* Name of input file, default: NULL.   */
    const char *outFileName;    /* Name of output file, default: NULL.  */
    output_t outputType;        /* Output file type, default: HTML_OUT. */
    FILE *inputFile;            /* Input file pointer, default: stdin.  */
    FILE *outputFile;           /* Output file pointer, default: stdout.*/
};


/**
 * check_output_type(arg) -- determine if arg is a valid output_t
 **
 *  TODO: Create a function in strings.c that will convert a string to
 *        lowercase -- then move the string handling there.
 *
 *  TODO: Move the @throws call to the caller, and return -1 if **no** 
 *        match was found.
 **
 * @throws -- throw_invalid_output_t_error()
 * @return -- the determined arg_t output constant
 ************************************************************************/
static output_t check_output_type(const char *arg)
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
    return HTML_OUT;
}


/************************************************************************
 * @section Main Function
 ************************************************************************/

/**
 * main(argc, argv) -- parse arguments, open files, and call parser
 **
 * @throws -- throw_multiple_input_files_error()
 * @return -- EXIT_SUCCESS on completion
 ************************************************************************/
int main(int argc, char **argv)
{
    struct progopts opts = {
        .inFileName     = NULL,         /* String name of input file.   */
        .outFileName    = NULL,         /* String name of output file.  */
        .outputType     = HTML_OUT,     /* Enumerated output type.      */
        .inputFile      = stdin,        /* Input file pointer.          */
        .outputFile     = stdout        /* Output file pointer.         */
    };
    markdown_t *queue   = NULL;         /* MD list returned by parser.  */
    int helpFlag        = 0;            /* Flag for help dialog.        */
    int versionFlag     = 0;            /* Flag for version dialog.     */
    
    while (true) {
        int optindex = 0;
        const struct option long_opts[] = {
          {"help",      no_argument,    &helpFlag,      1},
          {"version",   no_argument,    &versionFlag,   1},
          {0,           0,              0,              0},
        };
        
        /* Get character code or EOF for current argument. */
        int c = getopt_long(argc, argv, "o:t:", long_opts, &optindex);
        if (c == -1) break;
        
        switch (c) {
            case 'o':
                opts.outFileName = optarg;
                break;
            case 't':
                opts.outputType = check_output_type(optarg);
                break;
            default: break;
        }
    }
    
    /* Assign the remaining arguments to be the input file names. 
     * Currently, we only accept a single input file. Sending 
     * multiple files to patdown will throw a fatal error. */
    if (optind < argc)
    {
        if (!opts.inFileName) opts.inFileName = argv[optind++];
        else throw_multiple_input_files_error();
    }
    
    /* If both helpFlag and versionFlag were turned on during command-
     * line parsing, only print the help output dialog. */
    if (helpFlag) {
        printf("HELP!\n");
    }
    else if (versionFlag) {
        printf("VERSION!\n");
    }
    

    if (opts.inFileName) {
        opts.inputFile = open_file(opts.inFileName);
    }

    /* If opts.outFileName... open for reading. */

    queue = markdown(opts.inputFile);
    print_markdown_queue(queue);
    close_file(opts.inputFile);
    free_markdown(queue);
    
    return EXIT_SUCCESS;
}