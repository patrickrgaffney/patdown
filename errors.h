/* errors.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    02/17/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This file contains prototypes for helper functions that print
 * error messages to stdout.  These functions are called _mainly_
 * by the atexit() function in stdlib.h, which takes a pointer to a 
 * function, and executes that function immediately before program 
 * termination.
 */

#ifndef ERRORS_H
#define ERRORS_H

/* ==================================================================
 * == FUNCTIONS
 * =============================================================== */

/* Display USAGE dialog */
void printUsageMsg(void);


/* Display HELP dialog */
void printHelpMsg(void);


/* Display VERSION dialog */
void printVersionMsg(void);

/* Display INPUT FILE READ error */
void inputFileError(void);

/* Display MEMORY ERROR exit message. */
void printMemoryError(void);


#endif