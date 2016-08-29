/*****
 * strings.c -- string handling utilities
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-08-29
 * 
 *****************************************************************************/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "strings.h"
#include "errors.h"

#define NEWLINE 1
#define NULL_CHAR 1

/******************************************************************************
 * @section Basic String Handling Utilities
 *
 * These methods operate on basic `char *` strings.
 *****************************************************************************/

/*****
 * Allocate space for a new `char *` object.
 *
 * @param   size    Size of the new string (in characters).
 *
 * @throws  throw_memory_error()
 * @return  Newly allocated string.
 *****************************************************************************/
char *alloc_string(const size_t size)
{
    char *string = NULL;
    string = malloc(sizeof(char) * size);
    if (!string) throw_memory_error();
    return string;
}


/*****
 * Reallocate the size of a `char *` string.
 *
 * @param   s       Originally allocated string.
 * @param   size    Size (in characters) of the new string.
 *
 * @throws  throw_memory_error()
 * @return  A reallocated string.
 *****************************************************************************/
char *realloc_string(char *s, const size_t size)
{
    char *newstr = NULL;
    newstr = realloc(s, sizeof(char) * size);
    if (!newstr) throw_memory_error();
    return newstr;
}



/******************************************************************************
 * @section Methods for Operating on `string_t` Nodes
 *
 * These methods operate on `string_t` nodes -- as they were define in 
 * `strings.h`. These nodes provide a wrapper for a basic `char *` string in
 * order to hold some additional information about the string.
 *****************************************************************************/

/*****
 * Create a substring of a `string_t` node.
 *
 * @param   s       Original string.
 * @param   start   Index where the substring should start.
 * @param   stop    Index where the substring should stop.
 *
 * @warning If `s` is `NULL`, the `string_t` node what is returned will have
 *          all member initiated to `0` or `NULL`. So, this function never
 *          returns `NULL` directly, worst case, it returns a container 
 *          `string_t` node with all the members set to `NULL`.
 *
 * @return  Newly allocated and initialized substring.
 *****************************************************************************/
string_t *create_substring(string_t *s, size_t start, const size_t stop)
{
    string_t *newstr = NULL;
    size_t size = (stop - start) + 1; // Add 1 to be inclusive.
    if (!s) {
        newstr = init_stringt(0);
        return newstr; // newstr->string == NULL
    }
    
    newstr = init_stringt(size + NEWLINE);
    newstr->len = size;
    memcpy(newstr->string, s->string + start, size);
    newstr->string[size] = '\0';
    return newstr;
}


/*****
 * Allocate memory for a new `string_t` node.
 *
 * @todo    Remove the `size` parameter from this function -- it is not used.
 *
 * @throws  throw_memory_error()
 * @return  An allocated `string_t` node.
 *****************************************************************************/
static string_t *alloc_stringt(const size_t size)
{
    string_t *str = NULL;
    str = malloc(sizeof(string_t));
    if (!str) throw_memory_error();
    return str;
}


/*****
 * Initialize a new `string_t` node.
 *
 * @param   size    Size of the new string.
 *
 * @warning A `size` of `0` sets the `string` member to `NULL`.
 * @throws  throw_memory_error()
 * @return  An allocated and initialized `string_t` node.
 *****************************************************************************/
string_t *init_stringt(const size_t size)
{
    string_t *str = alloc_stringt(size);
    if (size == 0) {
        str->size   = 0;
        str->len    = 0;
        str->string = NULL;
    }
    else {
        str->size   = size;
        str->len    = 0;
        str->string = alloc_string(size);
    }
    return str;
}


/*****
 * Free the memory occupied by a `string_t` node.
 *
 * @param   s   The node to be free'd.
 *****************************************************************************/
void free_stringt(string_t *s)
{
    if (s) {
        if (s->string) free(s->string);
        free(s);
    }
}


/*****
 * Combine two `string_t` nodes into a single node.
 *
 * @param   fmt The format string for how these nodes will be combined.
 * @param   s1  The first string to be inserted into the return string.
 * @param   s2  The last string to be inserted into the return string.
 *
 * @warning If either `s1` or `s2` is `NULL`, this function will return `NULL`.
 * @return  A newly combined `string_t` node, or `NULL` if there was an error.
 *****************************************************************************/
string_t *combine_strings(const char *fmt, string_t *s1, string_t *s2)
{
    if (!s1 || !s2) return NULL;
    size_t size = s1->len + s2->len + 1;
    string_t *dest = init_stringt(size + NULL_CHAR);
    dest->len = size;
    snprintf(dest->string, size + NULL_CHAR, fmt, s1->string, s2->string);
    return dest;
}