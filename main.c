/**
 * main.c -- parse command-line arguments and open files
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2017-01-05
 *  project:    patdown
 * 
 ************************************************************************/

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "output.h"
#include "markdown.h"
#include "strings.h"

static const char *_program = "patdown";
static const char *_version = "0.0.1";
static const char *_author  = "Pat Gaffney";
static const char *_email   = "pat@hypepat.com";

/** Print the version dialog. */
static void print_version()
{
    printf("%s %s\n", _program, _version);
    printf("\n");
    printf("A Markdown compiler.\n");
    printf("Written by %s <%s>\n", _author, _email);
}


/** Print the help dialog. */
static void print_help()
{
    print_version();
    printf("\n");
    printf("  USAGE: %s [options <arg>] <inputfile>\n", _program);
    printf("\n");
    printf("  OPTIONS:\n");
    printf("  -5               Output HTML5 [default]\n");
    printf("  -d               Output parsing information\n");
    printf("  -h, --help       Show help\n");
    printf("  -o <file>        Set output file [default: stdout]\n");
    printf("  -v, --version    Show version\n");
    printf("\n");
    printf("\n");
}


/************************************************************************
 * # Opening & Closing Files
 ************************************************************************/

/**
 * Open a file stream for some given file access mode.
 *
 * - parameter fileName: The name of the file to be opened (a C-string).
 * - parameter type: The mode by which the file should be opened.
 *
 * - returns: A pointer to the file stream.
 */
static FILE *open_file(const char *fileName, const char *mode)
{
    FILE *filePtr = fopen(fileName, mode);
    if (!filePtr) {
        printf("FATAL: file could not be opened: \'%s\'\n", fileName);
        exit(EXIT_FAILURE);
    }
    return filePtr;
}


/************************************************************************
 * # Reading Input From Files
 ************************************************************************/

/**
 * Read all bytes from a supplied input file stream.
 *
 * Bytes are read into a `String` buffer that is reallocated by an order
 * of 5120 bytes in order to make room for the entire file.
 *
 * - parameter ifp: Input file stream (must be opened for reading).
 *
 * - returns: A `String` node initialized with the bytes read from file, 
 *   the memory allocated for this node (in bytes), and the number of bytes 
 *   actually read from the input file stream.
 */
static String *read_all_input_bytes(FILE *ifp)
{
    int ret    = 0;         /* The return value from fread(). */
    size_t lim = 5120;      /* Max number of bytes to allocate for. */
    size_t order  = 1;      /* Multiplier for lim if we realloc() memory. */
    String *bytes = init_string(lim);
    
    if (!ifp) return NULL;
    
    while (true) {
        ret = fread(bytes->data + bytes->length, 1, 
                    bytes->allocd - bytes->length, ifp);
        bytes->length += ret;
        
        if (feof(ifp)) break;
        else {
            lim += lim * ++order;
            realloc_string(bytes, lim);
        }
    }
    bytes->data[bytes->length] = '\0';
    return bytes;
}


/************************************************************************
 * # Main Function
 ************************************************************************/

/**
 * Begin program execution and parse command-line arguments.
 *
 * - throws fatal_input_files_error: Multiple input files provided.
 *
 * - returns: `EXIT_SUCCESS`.
 */
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
            case '5': outType = OUT_HTML5;  break;
            case 'd': outType = OUT_PARSED; break;
            case 'h': helpFlag = 1;         break;
            case 'o': oFileName = optarg;   break;
            case 'v': versionFlag = 1;      break;
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
    
    /* If both help and version flags were provided only print help. */
    if (helpFlag) print_help();
    else if (versionFlag) print_version();

    if (iFileName) ifp = open_file(iFileName, "r");
    if (oFileName) ofp = open_file(oFileName, "w");
    
    rawBytes = read_all_input_bytes(ifp);
    
    markdown(rawBytes);
    if (rawBytes) free_string(rawBytes);
    
    debug_print_queue();
    
    if (iFileName && ifp) fclose(ofp);
    if (oFileName && ofp) fclose(ofp);

    free_markdown();
    
    return EXIT_SUCCESS;
}
