/* 
 * utility.c -- utility methods for generic tasks
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 07/10/2016.
 * 
 *********ultrapatbeams*/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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
 * @return -- newly allocated and initialized substring or NULL
 ******************************************************************/
char *create_substring(char *s, size_t start, const size_t stop)
{
    if (!s) return NULL;
    char *newstr = NULL;
    
    size_t size = (stop - start) + 1; // Add 1 to be inclusive.
    newstr = alloc_string(size + NEWLINE);
    memcpy(newstr, s + start, size);
    newstr[size] = '\0';
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
 * combine_strings() -- append a string to the end of another
 *
 * char *s1 -- the string on which the append will happen
 * char *s2 -- the string which will be appended
 * const bool newline -- true puts a newline ('\n') between s1 and 
 *                       s2, false separates them with a space (' ')
 *
 * @return -- the combined string, which occurs on s1
 ******************************************************************/
char *combine_strings(char *s1, char *s2, const bool newline)
{
    if (!s1 || !s2) return NULL;
    const size_t s1len = strlen(s1);
    const size_t s2len = strlen(s2);
    const size_t size  = s1len + s2len + NEWLINE;
    char *dest = alloc_string(size + NULL_CHAR);
    
    memcpy(dest, s1, s1len);
    dest[s1len] = newline ? '\n' : ' ';
    memcpy(dest + s1len + NEWLINE, s2, s2len + NULL_CHAR);
    return dest;
}