/* errors.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 02/15/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file contains the definitions of functions that print dialogs for
 * the user either of exit from the program or on request via a command-
 * line flag argument.
 * ======================================================================= */

#ifndef ERRORS_H
#define ERRORS_H


/* print_usage()
 * =======================================================================
 * Print the usage dialog.
 * ======================================================================= */
void print_usage(void);


/* print_help()
 * =======================================================================
 * Print the help dialog.
 * ======================================================================= */
void print_help(void);


/* print_version()
 * =======================================================================
 * Print the version dialog.
 * ======================================================================= */
void print_version(void);


/* printf_file_error()
 * =======================================================================
 * Print the file error dialog.
 * ======================================================================= */
void input_file_error(void);


/* print_memory_error()
 * =======================================================================
 * Print the memory error dialog.
 * ======================================================================= */
void print_memory_error(void);


#endif