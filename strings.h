/* 
 * strings.h -- string handling utilities
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 07/20/2016.
 * 
 *********ultrapatbeams*/

#ifndef STRINGS_H
#define STRINGS_H

#include <stddef.h>
#include <stdbool.h>
#include "errors.h"


/******************************************************************
 * string_t() -- a dynamic string type
 *
 * char *string -- raw string read from file
 * size_t size  -- number of chars allocated to be stored
 * size_t len   -- number of chars stored in *string
*                  NOTE: this doesn't include the NULL char '\0'
 ******************************************************************/
typedef struct {
    char *string;
    size_t size;
    size_t len;
} string_t;


/******************************************************************
 * alloc_stringt() -- allocate space for a new string type 
 *
 * const size_t size -- size of the new string
 *
 * @throws -- throw_memory_error() if string cannot be allocated
 * @return -- an allocated string_t type
 ******************************************************************/
string_t *alloc_stringt(const size_t size);


/******************************************************************
 * init_stringt() -- initiate a new string type, string_t
 *
 * const size_t size -- size of the new string
 *
 * @throws -- throw_memory_error() if string cannot be allocated
 * @return -- an allocated and initialized string_t type
 ******************************************************************/
string_t *init_stringt(const size_t size);


/******************************************************************
 * free_stringt() -- free string_t node, if it exists
 *
 * string_t *s -- string node to be free'd
 *
 * @noreturn  -- control returned to the caller
 ******************************************************************/
void free_stringt(string_t *s);


/******************************************************************
 * create_substring() -- create a substring given parameters
 * 
 * char *s              -- original string
 * const size_t start   -- index where the substring should start
 * const size_t stop    -- index where the substring should end
 *
 * @return -- newly allocated and initialized substring
 ******************************************************************/
string_t *create_substring(string_t *s, size_t start, const size_t stop);


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
 * combine_strings() -- combine two string_t nodes into one
 *
 * const char *fmt -- the format string for snprintf()
 * string_t *s1 -- the leading string
 * string_t *s2 -- the trailing string
 * const size_t size -- # of characters to be written to new string
 * 
 * @return -- a new string_t node containing *s1 and *s2 as 
 *            dictated by the format string
 ******************************************************************/
string_t *combine_strings(const char *fmt, string_t *s1, string_t *s2);

#endif