/* 
 * utility.c -- utility methods for generic tasks
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 06/22/2016.
 * 
 *********ultrapatbeams*/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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
char *create_substring(char *s, size_t start, const size_t stop)
{
    if (!s) return NULL;
    char *newstr = NULL;
    
    if (stop <= start) start = stop - 1;
    size_t size = (stop - start) + 1;
    newstr = alloc_string(size);
    
    while (start <= stop) {
        *newstr++ = s[start++];
    }
    *newstr = '\0';
    return newstr - size;
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
    size_t s1len = strlen(s1);
    size_t s2len = strlen(s2);
    const size_t newstrlen = s1len + s2len + 1; // newline
    s1 = realloc_string(s1, newstrlen);
    
    if (newline) s1[s1len++] = '\n';
    else s1[s1len++] = ' ';
    while (s2len-- > 0) s1[s1len++] = *s2++;
    
    s1[s1len] = '\0';
    return s1;
}