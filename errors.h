/**
 * errors.h -- methods for throwing fatal and non-fatal errors
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2017-01-05
 *  project:    patdown
 * 
 ************************************************************************/

#pragma once

/** Memory could not be allocated. */
void throw_fatal_memory_error(void);
