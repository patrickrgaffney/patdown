/* 
 * errors.h -- methods for throwing errors.
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 06/15/2016.
 * 
 *********ultrapatbeams*/

#ifndef ERRORS_H
#define ERRORS_H

/******************************************************************
 * throw_memory_error() -- memory could not be allocated
 * 
 * @fatal -- exits program with error dialog
 ******************************************************************/
void throw_memory_error(void);


/******************************************************************
 * throw_file_opening_error() -- file could not be opened
 *
 * @fatal -- exits program with error dialog
 ******************************************************************/
void throw_file_opening_error(const char *fileName);


/******************************************************************
 * throw_invalid_option_error() -- provided option flag is invalid
 *
 * const char *argument -- the invalid option flag
 *
 * @fatal -- exits program with error dialog
 ******************************************************************/
void throw_invalid_option_error(const char *argument);


/******************************************************************
 * throw_invalid_output_t_error() -- output type is invalid
 *
 * const char *argument -- the invalid output type
 *
 * @fatal -- exits program with error dialog
 ******************************************************************/
void throw_invalid_output_t_error(const char *argument);


/******************************************************************
 * throw_multiple_input_files_error() -- multiple input files 
 *                                       provided
 *
 * @fatal -- exits program with error dialog
 ******************************************************************/
void throw_multiple_input_files_error(void);


#endif