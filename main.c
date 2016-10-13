/***** 
 * main.c -- parse command-line arguments and open files
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-10-05
 * 
 ************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "errors.h"
#include "files.h"
#include "output.h"
#include "strings.h"

static const char *_program = "patdown";
static const char *_version = "0.0.1";
static const char *_author  = "Pat Gaffney";
static const char *_email   = "pat@hypepat.com";

static void __print_version()
{
    fprintf(stderr, "%s Version %s\n", _program, _version);
    fprintf(stderr, "\n");
    fprintf(stderr, "-- A Markdown compiler.\n");
    fprintf(stderr, "-- Written by %s <%s>\n", _author, _email);
}

static void __print_help()
{
    __print_version();
    fprintf(stderr, "\n");
    fprintf(stderr, "  USAGE: %s [options <arg>] <inputfile>\n", _program);
    fprintf(stderr, "\n");
    fprintf(stderr, "  OPTIONS:\n");
    fprintf(stderr, "  -5               Output HTML5 [default]\n");
    fprintf(stderr, "  -d               Output parsing information\n");
    fprintf(stderr, "  -h, --help       Show help\n");
    fprintf(stderr, "  -o <file>        Set output file [default: stdout]\n");
    fprintf(stderr, "  -v, --version    Show version\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
}


/************************************************************************
 * @section Main Function
 ************************************************************************/

/*****
 * Begin program execution and parse command-line arguments.
 *
 * ERRORS
 *  throw_fatal_input_files_error   Multiple input files provided.
 *
 * RETURNS
 *  0 or EXIT_SUCCESS.
 *****/
int main(int argc, char **argv)
{   
    char *iFileName  = NULL;        /* Input file name. */
    FILE *ifp        = stdin;       /* Input file stream. */
    char *oFileName  = NULL;        /* Output file name. */
    FILE *ofp        = stdout;      /* Output file stream. */
    output_t outType = OUT_HTML5;   /* Output type. */
    int helpFlag     = 0;           /* Flag for help dialog. */
    int versionFlag  = 0;           /* Flag for version dialog. */
    String *rawBytes = NULL;        /* Raw bytes read from inputfile. */
    
    while (true) {
        int optindex = 0;
        const struct option long_opts[] = {
          {"help",      no_argument,    &helpFlag,      1},
          {"version",   no_argument,    &versionFlag,   1},
          {0,           0,              0,              0},
        };
        
        /* Get character code or EOF for current argument. */
        int c = getopt_long(argc, argv, "5dho:v", long_opts, &optindex);
        if (c == -1) break;
        
        switch (c) {
            case '5':   /* Change ouptut type to OUT_HTML5. */
                outType = OUT_HTML5;
                break;
            case 'd':   /* Change output type to OUT_PARSED. */
                outType = OUT_PARSED;
                break;
            case 'h':   /* Output help dialog. */
                helpFlag = 1;
                break;
            case 'o':   /* Set the output file. */
                oFileName = optarg;
                break;
            case 'v':   /* Output version dialog. */
                versionFlag = 1;
                break;
            default: break;
        }
    }
    
    /* Assign the remaining arguments to be the input file names. 
     * Currently, we only accept a single input file. Any filenames
     * provided after the first will be ignored. */
    while (optind < argc)
    {
        if (!iFileName) {
            iFileName = argv[optind++];
            break;
        }
    }
    
    /* If both helpFlag and versionFlag were turned on during command-
     * line parsing, only print the help output dialog. */
    if (helpFlag) __print_help();
    else if (versionFlag) __print_version();
    

    if (iFileName) {
        ifp = open_file(iFileName, "r");
    }
    if (oFileName) {
        ofp = open_file(oFileName, "w");
    }
    
    
    rawBytes = read_all_input_bytes(ifp);
    
    printf("-----bytes allocated for: %zu\n", rawBytes->allocd);
    printf("-----bytes read from file: %zu\n", rawBytes->length);
    printf("-----begin raw bytes:\n%s", rawBytes->data);
    printf("-----end raw bytes\n");

    if (iFileName) close_file(ifp);
    if (oFileName) close_file(ofp);
    if (rawBytes) free_string(rawBytes);

    
    return EXIT_SUCCESS;
}
