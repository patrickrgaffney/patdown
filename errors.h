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
 * error messages to stdout.  These functions are called exclusively
 * by the atexit() function in stdlib.h, which takes a pointer to a 
 * function, and executes that function immediately before program 
 * termination.
 */

/* ==================================================================
 * == FUNCTIONS
 * =============================================================== */

/* Display USAGE exit message */
void printUsageMsg(void);