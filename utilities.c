/* utilities.c
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 03/20/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file contains the implementations of functions that perform random
 * tasks that can be otherwise viewed as utility functions.
 * ======================================================================= */

#include <stdlib.h>
#include <string.h>

/* create_substring(const char *, size_t, size_t)
 * =======================================================================
 * Create a substring from the input string *s. The substring begins at 
 * s[start] and ends at s[stop], terminated by a NULL character.
 * 
 * Return NULL is s is NULL, or if stop is less than start. Otherwise 
 * return the new substring.
 * ======================================================================= */
char *create_substr(const char *s, size_t start, size_t stop)
{
    if (s == NULL) return NULL;
    
    if (stop < start) return NULL;
    size_t newstrlen = stop - start;

    char *newstr = (char *) malloc(sizeof(char) * newstrlen + 1);
    if (!newstr) return NULL;

    while (start < stop)
    {
        *newstr++ = s[start++];
    }
    *newstr = '\0';
    return newstr - newstrlen;
}


/* combine_strings_newline(const char *, const char *)
 * =======================================================================
 * Combine two strings, s1 and s2, into a new string, newstr, separated by
 * a newline. The new string is terminated by a NULL character.
 *
 * Return NULL if s1 or s2 are NULL, or if memory cannot be allocated.
 * Otherwise return the new string.
 * ======================================================================= */
char *combine_strings_newline(const char *s1, const char *s2)
{
    if (s1 == NULL || s2 == NULL) return NULL;
    
    size_t s1len = strlen(s1);
    size_t s2len = strlen(s2);
    const size_t newstrlen = s1len + s2len + 1; // Add newline.
    
    char *newstr = (char *) malloc(sizeof(char) * (newstrlen + 1));
    if (!newstr) return NULL;
    
    while (s1len-- > 0) *newstr++ = *s1++;
    *newstr++ = '\n';
    
    while (s2len-- > 0) *newstr++ = *s2++;
    *newstr = '\0';
    
    return newstr - newstrlen;
}