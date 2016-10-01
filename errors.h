/***** 
 * errors.h -- methods for throwing fatal and non-fatal errors
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-15
 * 
 ************************************************************************/

#ifndef ERRORS_H
#define ERRORS_H

/************************************************************************
 * @section Fatal Errors
 *
 *  All of these errors cause program execution to be halted.
 ************************************************************************/

/* Memory could not be allocated */
void throw_memory_error(void);

/* File could not be opened */
void throw_file_opening_error(const char *file);

/* Multiple input files provided */
void throw_multiple_input_files_error(void);


/************************************************************************
 * @section Non-Fatal Errors
 *
 *  None of these errors cause program execution to be halted. Instead, 
 *  they print out an error dialog and return control to the caller.
 ************************************************************************/

/* Unknown output type. */
void throw_invalid_output_type(const char *type)

#endif