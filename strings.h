/*****
 * strings.c -- string handling utilities
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-15
 * 
 ************************************************************************/

#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>
#include <stddef.h>

#include "errors.h"


/************************************************************************
 * @section Basic String Handling Utilities
 **
 * These methods operate on basic `char *` strings.
 ************************************************************************/

/* Allocate space for a size-character string. */
char *alloc_string(const size_t size);

/* Reallocate the size of s. */
char *realloc_string(char *s, const size_t size);


/************************************************************************
 * @section string_t Nodes
 **
 *  These methods operate on string_t nodes -- as they were defined in 
 *  strings.h. These nodes provide a wrapper for a basic `char *` string
 *  in order to hold some additional information about the string.
 ************************************************************************/

/**
 * string_t -- a wrapper type for `char *` strings
 ************************************************************************/
typedef struct 
{
    char *string;   /* The actual character array to be stored.         */
    size_t size;    /* TODO: remove this node.                          */
    size_t len;     /* The number of character currently stored.        */
} string_t;

/* Initialize a new string_t node. */
string_t *init_stringt(const size_t size);

/* Free the memory occupied by s. */
void free_stringt(string_t *s);

/* Create substring of a string_t node. */
string_t *create_substring(string_t *s, size_t start, const size_t stop);

/* Combine s1 and s2 into a single string. */
string_t *combine_strings(const char *fmt, string_t *s1, string_t *s2);

#endif