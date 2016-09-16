/***** 
 * errors.h -- methods for throwing errors
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-15
 * 
 ************************************************************************/

#ifndef ERRORS_H
#define ERRORS_H

/* Memory could not be allocated */
void throw_memory_error(void);

/* File could not be opened */
void throw_file_opening_error(const char *fileName);

/* Multiple input files provided */
void throw_multiple_input_files_error(void);

#endif