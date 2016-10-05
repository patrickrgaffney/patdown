/***** 
 * main.c -- parse command-line arguments and open files
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-10-04
 * 
 ************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "errors.h"
#include "files.h"
#include "strings.h"

static const char *program = "patdown";
static const char *version = "0.0.1";


/************************************************************************
 * Command-line Argument Parsing
 ************************************************************************/

/*****
 * Valid ouput type constants.
 *
 *  These constants are used to differentiate between the valid output 
 *  types supported by patdown.
 * 
 *  TODO: Add support for more types: RTF, PDF (...maybe...)
 *****/
typedef enum 
{
    OUT_HTML5,      /* Default: HTML5 element syntax. */ 
    OUT_PARSED      /* Internal parsing information (for debugging). */
} output_t;


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
    if (helpFlag) {
        printf("HELP!\n");
    }
    else if (versionFlag) {
        printf("VERSION!\n");
    }
    

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
