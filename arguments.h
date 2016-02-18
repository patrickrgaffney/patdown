/* arguments.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    02/17/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This file contains helpers functions, structures, and enumerations
 * that aid in the parsing of command-line arguments.
 */

/* ==================================================================
 * == ENUMERATIONS
 * =============================================================== */

/* Values for different command-line arguments. These values are
 * 'assigned' to each argument passed to the program, and they
 * represent the various valid arguments.
 *
 * For example, if you created an array `argt[]` with size `argc`, 
 * then for every `argv[i]` there is a corresponding value `argt[i]`.
 */
enum argumentTypes 
{
    PROGRAM_NAME,
    INPUT_FILE_NAME,
    OUTPUT_FILE_FLAG,
    OUTPUT_FILE_NAME,
    OUTPUT_FORMAT_FLAG,
    OUTPUT_FORMAT_NAME,
    HELP_FLAG,
    VERSION_FLAG
};

/* Synonym for `enum argumentTypes` */
typedef enum argumentTypes ArgType;


/* Raw values of the valid command-line flags. */
struct argumentFlags
{
    char *help;
    char *version;
    char *output;
    char *format;
};

/* Synonym for `struct argumentFlags` */
typedef struct argumentFlags Flag;

/* ==================================================================
 * == FUNCTIONS
 * =============================================================== */

/* If arguments were passed to the program at execution, parse them
 * to determine the programs purpose. Return an array of size `argc`
 * where each element contains the ArgType of the corresponding 
 * argument, i.e:
 *
 *  argt[i] is the ArgType of argv[i]
 */
ArgType *parseArguments(const int argc, const char *argv[]);


/* Test a string against the variety of possible command-line
 * arguments, return the purpose of that specific argument.
 */
ArgType testProgramArgument(const char argument[], const ArgType prevArg);


/* Compare the input string with the different possible command-
 * line arguments (flags) and return the ArgType of that flag. Exit
 * the program if the flag is not recognizable.
 */
ArgType checkFlags(const char possibleFlag[]);


/* Makes an educated guess about the ArgType of the command-line
 * argument based on the argument that preceeded it.
 */
ArgType checkNonFlags(const ArgType prevArg, const char argument[]);