/*****
 * strings.c -- string handling utilities
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-10-01
 * 
 ************************************************************************/

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "strings.h"

#define NEWLINE 1
#define NULL_CHAR 1

/************************************************************************
 * @section Basic String Handling Utilities
 **
 * These methods operate on basic `char *` strings.
 ************************************************************************/

/** Allocate space for a character array of size characters. ************/
char *alloc_char_array(const size_t size)
{
    char *string = NULL;
    string = malloc(sizeof(char) * size);
    if (!string) throw_fatal_memory_error();
    return string;
}

/** Reallocate the size of s to hold size characters. *******************/
char *realloc_char_array(char *s, const size_t size)
{
    char *newstr = NULL;
    newstr = realloc(s, sizeof(char) * size);
    if (!newstr) throw_fatal_memory_error();
    return newstr;
}

/* Return the lowercase version of upper. *******************************/
char *get_lowercase_char_array(const char *upper)
{
    if (!upper) return NULL;
    size_t size = strlen(upper);
    char *lower = alloc_char_array(size + NULL_CHAR);
    
    while (*upper) *lower++ = tolower(*upper++);
    *lower = '\0';

    return lower - size;
}

/************************************************************************
 * @section String Nodes
 *
 *  These methods operate on String nodes -- as they were defined in 
 *  strings.h. These nodes provide a wrapper for a basic `char *` string
 *  in order to hold some additional information about the string.
 ************************************************************************/

/** Allocate space for a new String node. *******************************/
static String *alloc_stringt(const size_t size)
{
    String *str = NULL;
    str = malloc(sizeof(String));
    if (!str) throw_fatal_memory_error();
    return str;
}


/** String API: Initialize a new String node ****************************/
String *init_string(const size_t size)
{
    String *str = alloc_stringt(size);
    
    if (size == 0) {
        str->size   = 0;
        str->string = NULL;
    }
    else {
        str->size   = size;
        str->string = alloc_char_array(size);
    }
    str->len = 0;
    return str;
}


/** Free the memory occupied by s. **************************************/
void free_string(String *s)
{
    if (s) {
        if (s->string) free(s->string);
        free(s);
    }
}


/** Create substring of a s from s[start] to s[stop]. *******************/
String *create_substring(String *s, size_t start, const size_t stop)
{
    String *newstr = NULL;              /* String node to be returned. */
    size_t size = (stop - start) + 1;   /* Add 1 to be inclusive. */
    
    /* If the original String is NULL, create then create a string. */
    if (!s) return init_string(0);
    
    newstr = init_string(size + NEWLINE);
    newstr->len = size;
    memcpy(newstr->string, s->string + start, size);
    newstr->string[size] = '\0';
    return newstr;
}


/** Combine s1 and s2 into a single String according to fmt. ************/
String *combine_strings(const char *fmt, String *s1, String *s2)
{
    if (!s1 || !s2) return NULL;
    
    /* Get size of the new string then allocate space for it. */
    size_t size    = s1->len + s2->len + 1;
    String *dest = init_string(size + NULL_CHAR);
    
    dest->len = size;
    snprintf(dest->string, size + NULL_CHAR, fmt, s1->string, s2->string);
    return dest;
}