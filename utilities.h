/* utilities.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 03/20/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file contains the definitions of functions that perform random
 * tasks that can be otherwise viewed as utility functions.
 * ======================================================================= */

#ifndef UTILITIES_H
#define UTILITIES_H


/* create_substring(const char *, size_t, size_t)
 * =======================================================================
 * Create a substring from the input string *s. The substring begins at 
 * s[start] and ends at s[stop], terminated by a NULL character.
 * 
 * Return NULL is s is NULL, or if stop is less than start. Otherwise 
 * return the new substring.
 * ======================================================================= */
char *create_substr(const char *s, size_t start, size_t stop);


/* combine_strings_newline(const char *, const char *)
 * =======================================================================
 * Combine two strings, s1 and s2, into a new string, newstr, separated by
 * a newline. The new string is terminated by a NULL character.
 *
 * Return NULL if s1 or s2 are NULL, or if memory cannot be allocated.
 * Otherwise return the new string.
 * ======================================================================= */
char *combine_strings_newline(const char *s1, const char *s2);


#endif