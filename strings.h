/*****
 * strings.c -- string handling utilities
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-30
 * 
 ************************************************************************/

#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>
#include <stddef.h>

#include "errors.h"


/************************************************************************
 * @section Basic String Handling Utilities
 *
 * These methods operate on basic `char *` strings.
 ************************************************************************/

/* Allocate space for a size-character string. */
char *alloc_char_array(const size_t size);

/* Reallocate the size of s. */
char *realloc_char_array(char *s, const size_t size);


/************************************************************************
 * @section String Nodes
 *
 *  These methods operate on String nodes -- as they were defined in 
 *  strings.h. These nodes provide a wrapper for a basic `char *` string
 *  in order to hold some additional information about the string.
 ************************************************************************/

/** String -- a wrapper type for `char *` strings **/
typedef struct String
{
    size_t size;    /* The size of the memory allocated -- in bytes. */
    char *string;   /* The actual character array to be stored. */
    size_t len;     /* The number of character currently stored. */
} String;

/* Initialize a new string_t node. */
String *init_string(const size_t size);

/* Free the memory occupied by s. */
void free_string(String *s);

/* Create substring of a String node. */
String *create_substring(String *s, size_t start, const size_t stop);

/* Combine s1 and s2 into a single string. */
String *combine_strings(const char *fmt, String *s1, String *s2);

#endif