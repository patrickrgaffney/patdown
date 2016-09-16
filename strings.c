/*****
 * strings.c -- string handling utilities
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-15
 * 
 ************************************************************************/

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

/*****
 * alloc_string(size) -- allocate space for a size-character string
 **
 * @throws -- throw_memory_error()
 * @return -- a pointer to the allocated string
 ************************************************************************/
char *alloc_string(const size_t size)
{
    char *string = NULL;
    string = malloc(sizeof(char) * size);
    if (!string) throw_memory_error();
    return string;
}


/**
 * realloc_string(s, size) -- reallocate the size of s
 **
 * @throws -- throw_memory_error()
 * @return -- a pointer to the reallocated string
 ************************************************************************/
char *realloc_string(char *s, const size_t size)
{
    char *newstr = NULL;
    newstr = realloc(s, sizeof(char) * size);
    if (!newstr) throw_memory_error();
    return newstr;
}


/************************************************************************
 * @section string_t Nodes
 **
 *  These methods operate on string_t nodes -- as they were defined in 
 *  strings.h. These nodes provide a wrapper for a basic `char *` string
 *  in order to hold some additional information about the string.
 ************************************************************************/

/**
 * alloc_stringt(size) -- allocate space for a new string_t node
 **
 *  TODO: Remove the size parameter from this function -- it is not used.
 **
 * @throws -- throw_memory_error()
 * @return -- a pointer to a string_t node
 ************************************************************************/
static string_t *alloc_stringt(const size_t size)
{
    string_t *str = NULL;
    str = malloc(sizeof(string_t));
    if (!str) throw_memory_error();
    return str;
}


/**
 * init_stringt(size) -- initialize a new string_t node
 **
 *  If size is 0, the node still gets created, but its values are NULL.
 *
 *  TODO: Remove the str->size member.
 **
 * @throws  throw_memory_error()
 * @return  An allocated and initialized `string_t` node.
 ************************************************************************/
string_t *init_stringt(const size_t size)
{
    string_t *str = alloc_stringt(size);
    
    if (size == 0) {
        str->size   = 0;
        str->string = NULL;
    }
    else {
        str->size   = size;
        str->string = alloc_string(size);
    }
    str->len    = 0;
    return str;
}


/**
 * free_stringt(s) -- free the memory occupied by s
 ************************************************************************/
void free_stringt(string_t *s)
{
    if (s) {
        if (s->string) free(s->string);
        free(s);
    }
}


/**
 * create_substring(s, start, stop) -- create substring of a string_t node
 **
 *  If s is NULL, the string_t node that is returned will have 
 *  all members initiated to 0 or NULL. So, this function never 
 *  returns NULL directly -- worst case, it returns a container 
 *  string_t node with all members set to NULL.
 *
 *  TODO: determine if the who free's the s node -- caller or function.
 **
 * @return -- a pointer to the newly allocated substring
 ************************************************************************/
string_t *create_substring(string_t *s, size_t start, const size_t stop)
{
    string_t *newstr = NULL;            /* String node to be returned.  */
    size_t size = (stop - start) + 1;   /* Add 1 to be inclusive.       */
    
    if (!s) {
        /* Create node, but initialize newstr->string to NULL. */
        newstr = init_stringt(0);
        return newstr;
    }
    
    newstr = init_stringt(size + NEWLINE);
    newstr->len = size;
    memcpy(newstr->string, s->string + start, size);
    newstr->string[size] = '\0';
    return newstr;
}


/**
 * combine_strings(fmt, s1, s2) -- combine s1 and s2 into a single string
 **
 *  The fmt string is the format string used by snprintf() to combine
 *  s1 and s2. If either s1 or s2 is NULL, this function returns NULL.
 **
 * @return -- a pointer to the new string node, or NULL
 *****************************************************************************/
string_t *combine_strings(const char *fmt, string_t *s1, string_t *s2)
{
    if (!s1 || !s2) return NULL;
    
    /* Get size of the new string then allocate space for it. */
    size_t size    = s1->len + s2->len + 1;
    string_t *dest = init_stringt(size + NULL_CHAR);
    
    dest->len = size;
    snprintf(dest->string, size + NULL_CHAR, fmt, s1->string, s2->string);
    return dest;
}