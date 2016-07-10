/* 
 * utility.h -- utility methods for generic tasks
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 06/22/2016.
 * 
 *********ultrapatbeams*/

#ifndef UTILITY_H
#define UTILITY_H

#include <stddef.h>
#include <stdbool.h>
#include "utility.h"
#include "errors.h"


/******************************************************************
 * create_substring() -- create a substring given parameters
 * 
 * char *s              -- original string
 * const size_t start   -- index where the substring should start
 * const size_t stop    -- index where the substring should end
 *
 * @return -- newly allocated and initialized substring
 ******************************************************************/
char *create_substring(char *s, size_t start, const size_t stop);


/******************************************************************
 * alloc_string() -- allocate space for a new string
 *
 * const size_t size -- size of the new string
 *
 * @throws -- throw_memory_error() if string cannot be allocated
 * @return -- an allocated string
 ******************************************************************/
char *alloc_string(const size_t size);

/******************************************************************
 * realloc_string() -- reallocate size of a string
 *
 * char *s           -- originally allocated string
 * const size_t size -- new size of the string
 *
 * @throws -- throw_memory_error() if string cannot be reallocated
 * @return -- a reallocated string
 ******************************************************************/
char *realloc_string(char *s, const size_t size);


/******************************************************************
 * combine_strings() -- append a string to the end of another
 *
 * char *s1 -- the string on which the append will happen
 * char *s2 -- the string which will be appended
 * const bool newline -- true puts a newline ('\n') between s1 and 
 *                       s2, false separates them with a space (' ')
 *
 * @return -- the combined string, which occurs on s1
 ******************************************************************/
char *combine_strings(char *s1, char *s2, const bool newline);

#endif