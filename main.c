/***** 
 * main.c -- parse command-line arguments and open files
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-30
 * 
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>

#include "errors.h"
#include "html.h"
#include "files.h"
#include "markdown.h"
#include "parsers.h"
#include "strings.h"


/************************************************************************
 * @section Command-line Argument Parsing
 ************************************************************************/

/** output_t: valid output type constants *******************************/
typedef enum 
{
    HTML_OUT    /* Default: HTML5 element syntax. */
} output_t;


/** ProgramOpts: a container for various program options ****************/
typedef struct
{
    const char *inFileName;     /* Name of input file, default: NULL. */
    const char *outFileName;    /* Name of output file, default: NULL. */
    output_t outputType;        /* Output file type, default: HTML_OUT. */
    FILE *inputFile;            /* Input file pointer, default: stdin. */
    FILE *outputFile;           /* Output file pointer, default: stdout.*/
} ProgramOpts;


/** Determine if arg is a valid output type. ****************************/
static output_t check_output_type(const char *arg)
{
    output_t type  = HTML_OUT;  /* Default to HTML output. */
    char *lowerArg = get_lowercase_char_array(arg);

    if (strcmp(lowerArg, "html") == 0) {
        type = HTML_OUT;
    }
    else throw_invalid_output_type(arg);
    
    free(lowerArg);
    return HTML_OUT;
}


/************************************************************************
 * @section Main Function
 ************************************************************************/

/** Parse arguments, open files, and call parser. ***********************/
int main(int argc, char **argv)
{
    ProgramOpts opts = {
        .inFileName  = NULL,        /* String name of input file. */
        .outFileName = NULL,        /* String name of output file. */
        .outputType  = HTML_OUT,    /* Enumerated output type. */
        .inputFile   = stdin,       /* Input file pointer. */
        .outputFile  = stdout       /* Output file pointer. */
    };
    Markdown *queue  = NULL;        /* MD list returned by parser. */
    int helpFlag     = 0;           /* Flag for help dialog. */
    int versionFlag  = 0;           /* Flag for version dialog. */
    
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
        else throw_fatal_input_files_error();
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
        opts.inputFile = open_file(opts.inFileName, "r");
    }
    
    if (opts.outFileName) {
        opts.inputFile = open_file(opts.inFileName, "w");
    }


    queue = markdown(opts.inputFile);
    // print_markdown_queue(queue);
    output_html(opts.outputFile, queue);
    close_file(opts.inputFile);
    free_markdown(queue);
    
    return EXIT_SUCCESS;
}