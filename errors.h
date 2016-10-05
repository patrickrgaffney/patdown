/***** 
 * errors.h -- methods for throwing fatal and non-fatal errors
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-10-04
 * 
 ************************************************************************/

#ifndef __MD_ERRORS_H__
#define __MD_ERRORS_H__

/************************************************************************
 * Fatal Errors
 *
 *  All of these errors cause program execution to be halted.
 ************************************************************************/

/** Memory could not be allocated. **/
void throw_fatal_memory_error(void);

/** File could not be opened. **/
void throw_fatal_fopen_error(const char *file);


/************************************************************************
 * Non-Fatal Errors
 *
 *  None of these errors cause program execution to be halted. Instead, 
 *  they print out an error dialog and return control to the caller.
 ************************************************************************/

#endif
