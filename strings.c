/* 
 * strings.h -- string handling utilities
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 07/13/2016.
 * 
 *********ultrapatbeams*/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "strings.h"
#include "errors.h"

#define NEWLINE 1
#define NULL_CHAR 1


/******************************************************************
 * create_substring() -- create a substring given parameters
 * 
 * char *s              -- original string
 * const size_t start   -- index where the substring should start
 * const size_t stop    -- index where the substring should end
 *
 * @return -- newly allocated and initialized substring
 * @note   -- If *s is NULL, returns an allocated string_t node 
 *            with all members initiated to NULL or 0.
 ******************************************************************/
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


/******************************************************************
 * alloc_string() -- allocate space for a new string
 *
 * const size_t size -- size of the new string
 *
 * @throws -- throw_memory_error() if string cannot be allocated
 * @return -- an allocated string
 ******************************************************************/
char *alloc_string(const size_t size)
{
    char *string = NULL;
    string = malloc(sizeof(char) * size);
    if (!string) throw_memory_error();
    return string;
}


/******************************************************************
 * alloc_stringt() -- allocate space for a new string type 
 *
 * const size_t size -- size of the new string
 *
 * @throws -- throw_memory_error() if string cannot be allocated
 * @return -- an allocated string_t type
 ******************************************************************/
string_t *alloc_stringt(const size_t size)
{
    string_t *str = NULL;
    str = malloc(sizeof(string_t));
    if (!str) throw_memory_error();
    return str;
}


/******************************************************************
 * init_stringt() -- initiate a new string type, string_t
 *
 * const size_t size -- size of the new string (0 == NULL string)
 *
 * @throws -- throw_memory_error() if string cannot be allocated
 * @return -- an allocated and initialized string_t type
 ******************************************************************/
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


/******************************************************************
 * free_stringt() -- free string_t node, if it exists
 *
 * string_t *s -- string node to be free'd
 *
 * @noreturn  -- control returned to the caller
 ******************************************************************/
void free_stringt(string_t *s)
{
    if (s) {
        if (s->string) free(s->string);
        free(s);
    }
}


/******************************************************************
 * realloc_string() -- reallocate size of a string
 *
 * char *s           -- originally allocated string
 * const size_t size -- new size of the string
 *
 * @throws -- throw_memory_error() if string cannot be reallocated
 * @return -- a reallocated string
 ******************************************************************/
char *realloc_string(char *s, const size_t size)
{
    char *newstr = NULL;
    newstr = realloc(s, sizeof(char) * size);
    if (!newstr) throw_memory_error();
    return newstr;
}


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
string_t *combine_strings(const char *fmt, string_t *s1, string_t *s2, const size_t size)
{
    if (!s1 || !s2) return NULL;
    string_t *dest = init_stringt(size + NULL_CHAR);
    dest->len = size;
    snprintf(dest->string, size + NULL_CHAR, fmt, s1->string, s2->string);
    
    // free s1 -- it is being overwritten on return
    free_stringt(s1);
    return dest;
}