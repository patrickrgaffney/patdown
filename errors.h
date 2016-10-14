/***** 
 * errors.h -- methods for throwing fatal and non-fatal errors
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2016-10-13
 *  project:    patdown
 * 
 ************************************************************************/

#pragma once

/************************************************************************
 * Fatal Errors
 *
 *  All of these errors cause program execution to be halted.
 ************************************************************************/

/** Memory could not be allocated. **/
void throw_fatal_memory_error(void);

/** File could not be opened. **/
void throw_fatal_fopen_error(const char *file);
