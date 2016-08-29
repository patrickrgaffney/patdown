/*****
 * strings.c -- string handling utilities
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-08-29
 * 
 *****************************************************************************/

#ifndef STRINGS_H
#define STRINGS_H

#include <stddef.h>
#include <stdbool.h>
#include "errors.h"


/*****
 * A dynamic string (`char *`) wrapper-type.
 *
 * @member  string  The actual character array (string) stored.
 * @member  len     The number of character stored in `string`.
 *
 * @todo    Remove the `str->size` member from this type. It is not used, and
 *          we get the same functionality from `str->len`.
 *****************************************************************************/
typedef struct {
    char *string;
    size_t size;
    size_t len;
} string_t;


/******************************************************************************
 * @section Basic String Handling Utilities
 *
 * These methods operate on basic `char *` strings.
 *****************************************************************************/
char *alloc_string(const size_t size);
char *realloc_string(char *s, const size_t size);


/******************************************************************************
 * @section Methods for Operating on `string_t` Nodes
 *
 * These methods operate on `string_t` nodes -- as they were define in 
 * `strings.h`. These nodes provide a wrapper for a basic `char *` string in
 * order to hold some additional information about the string.
 *****************************************************************************/
string_t *init_stringt(const size_t size);
void free_stringt(string_t *s);
string_t *create_substring(string_t *s, size_t start, const size_t stop);
string_t *combine_strings(const char *fmt, string_t *s1, string_t *s2);

#endif