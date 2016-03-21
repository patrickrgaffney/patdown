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

#ifndef ARGUMENTS_H
#define ARGUMENTS_H


/* argtype_t
 * =======================================================================
 * Valid values for different argument types. Each of these values is 
 * loaded into an array argt whose indexes correspond to those of argv.
 * Then the deteremined type of argument argv[i] is argt[i].
 * ======================================================================= */
typedef enum argumentTypes 
{
    PROGRAM_NAME,
    INPUT_FILE_NAME,
    OUTPUT_FILE_FLAG,
    OUTPUT_FILE_NAME,
    OUTPUT_FORMAT_FLAG,
    OUTPUT_FORMAT_NAME,
    HELP_FLAG,
    VERSION_FLAG
} argtype_t;


/* formattype_t
 * =======================================================================
 * Valid values for different output formats as specified by the
 * OUTPUT_FILE_NAME.
 * ======================================================================= */
typedef enum formatTypes 
{ 
    HTML_FORMAT,
    TXT_FORMAT
} formattype_t;


/* Flag
 * =======================================================================
 * Conveninence type that is assigned the values of each valid string for
 * each valid command-line flag argument. One variable of type Flag is
 * statically initialized so that we can use it for comparison.
 * ======================================================================= */
typedef struct argumentFlags
{
    char *help;
    char *version;
    char *output;
    char *format;
} Flag;


/* Format
 * =======================================================================
 * Convenience tpye that is assigned the values of each valid string for
 * each valid output format argument. One variable of type Format is
 * statically initialized so that we can use it for comparison.
 * ======================================================================= */
typedef struct outputFormats
{
    char *html;
    char *htmlUp;
    char *txt;
    char *txtUp;
} Format;


/* parse_arguments(const int, const char **)
 * =======================================================================
 * Build an array of argtype_t's, such that the argument argv[i] has an
 * argtype_t of argt[i].
 *
 * Return a pointer to the newly initialized array of argtype_t's.
 * ======================================================================= */
argtype_t *parse_arguments(const int argc, const char *argv[]);


/* test_argument(const char *, const argtype_t)
 * =======================================================================
 * Determine if an argument is a flag or nonflag.
 *
 * Return a call to check_flags() or check_non_flags() based on the 
 * determination. Each of these functions will return an argtype_t of the
 * given argument to the caller of this function.
 * ======================================================================= */
argtype_t test_argument(const char *argument, const argtype_t prevArg);


/* check_flags(const char *)
 * =======================================================================
 * Comare the arguments we know to be a flag with each of the valid flags
 * to determine its argtype_t.
 *
 * Return the argtype_t of the given flag.
 * NOTE: This function will exit with an error message and code if the 
 *       flag is determined to be invalid.
 * ======================================================================= */
argtype_t check_flags(const char *possibleFlag);


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
argtype_t check_non_flags(const argtype_t prevArg, const char *argument);


/* check_non_flags(const argtype_t, const char *)
 * =======================================================================
 * Tests an arguments argtype to be sure that if the preceding argument
 * was a flag, that the current argument is a non-flag. If the test fails,
 * this function will exit the program with an error message and code.
 * ======================================================================= */
void test_argtype_for_match(const argtype_t testarg, const argtype_t reqarg, const char *errstr);
    

/* test_format_type(const char *)
 * =======================================================================
 * Tests a OUTPUT_FORMAT_NAME argument to determine if it is a valid
 * formattype_t.
 *
 * Return the formattype_t of arg.
 * NOTE: This function will exit if the arg in question is found to be an
 *       invalid formattype_t.
 * ======================================================================= */
formattype_t test_format_type(const char *arg);


/* check_args_for_errors(const int, const char *, const argtype_t *)
* =======================================================================
* Driver for the various test_*() functions. Determines if all the 
* arguments provided are valid, and that each flag that requires another
* argument has it.
*
* NOTE: This function will exit with a message and code if any of its
*       tests fails. Failing these tests is fatal.
* ======================================================================= */
void check_args_for_errors(const int argc, const char *argv[], const argtype_t *argt);


#endif