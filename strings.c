/*****
 * strings.c -- string handling utilities
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2016-10-13
 *  project:    patdown
 * 
 ************************************************************************/

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>

#include "errors.h"
#include "strings.h"


/************************************************************************
 * Data Array Utilities
 *
 *  A data array is defined to be an array of uint8_t elements.
 ************************************************************************/

/*****
 * Allocate memory for a data array of size elements.
 *
 * ARGUMENTS
 *  size    The number of elements to be stored in the array.
 *
 * ERRORS
 *  fatal_memory_error  Memory could not be allocated.
 *
 * RETURNS
 *  A pointer to the new data array.
 *****/
static uint8_t *__alloc_data_array(const size_t size)
{
    uint8_t *data = NULL;
    data = malloc(sizeof(uint8_t) * size);
    if (!data) throw_fatal_memory_error();
    return data;
}


/*****
 * Count the WS from *data to the first non-WS character.
 *
 *  Here, WS is "white space".
 *
 * ARGUMENTS
 *  data    An array of byte data (utf8 string).
 *
 * RETURNS
 *  The absolute number of WS characters encountered before a non-WS
 *  character, where a space is one and a tab is four.
 *****/
size_t count_indentation(uint8_t *data)
{
    size_t ws = 0;
    while (isblank(*data)) {
        if (*data++ == 0x20) ws++;
        else ws += 4;
    }
    return ws;
}


/************************************************************************
 * String Nodes
 ************************************************************************/

/*****
 * Allocate memory for a String node.
 *
 * ERRORS
 *  fatal_memory_error  Memory could not be allocated.
 *
 * RETURNS
 *  A pointer to the new String node.
 *****/
static String *__alloc_string_container(void)
{
    String *s = NULL;
    s = malloc(sizeof(String));
    if (!s) throw_fatal_memory_error();
    return s;
}

/*****
 * Allocate a String node to store size bytes.
 *
 *  If the size passed to this function is zero (0), it will allocate 
 *  and return a node that has all of it's members initialized to zero.
 *
 * ARGUMENTS
 *  size    The number of bytes to allocate for the data member.
 *
 * RETURNS
 *  A pointer to the new String node.
 *****/
String *init_string(const size_t size)
{
    String *str = __alloc_string_container();
    
    if (size == 0) {
        str->allocd = 0;
        str->data   = NULL;
    }
    else {
        str->allocd = size;
        str->data   = __alloc_data_array(size);
    }
    str->length = 0;
    return str;
}

/*****
 * Reallocate a String node's data member to contain size elements.
 *
 * ARGUMENTS
 *  str     The String node whose member should be reallocated.
 *  size    The new size (in bytes) of the requested memory.
 *****/
void realloc_string(String *str, const size_t size)
{
    str->data = realloc(str->data, sizeof(uint8_t) * size);
    str->allocd = size;
    if (!str->data) throw_fatal_memory_error();
}

/*****
 * Free the memory allocated for a String node, if it exists.
 *
 *  String nodes have a dynamically allocated member, `data`
 *  that is also free'd, if it exists.
 *
 * ARGUMENTS
 *  str     The String node to be free'd.
 *****/
void free_string(String *str)
{
    if (str) {
        if (str->data) free(str->data);
        free(str);
    }
}


/** Create substring of a s from s[start] to s[stop]. *******************/
// String *create_substring(String *s, size_t start, const size_t stop)
// {
//     String *newstr = NULL;              /* String node to be returned. */
//     size_t size = (stop - start) + 1;   /* Add 1 to be inclusive. */
//
//     /* If the original String is NULL, create then create a string. */
//     if (!s) return init_string(0);
//
//     newstr = init_string(size + NEWLINE);
//     newstr->len = size;
//     memcpy(newstr->string, s->string + start, size);
//     newstr->string[size] = '\0';
//     return newstr;
// }


/** Combine s1 and s2 into a single String according to fmt. ************/
// String *combine_strings(const char *fmt, String *s1, String *s2)
// {
//     if (!s1 || !s2) return NULL;
//
//     /* Get size of the new string then allocate space for it. */
//     size_t size    = s1->len + s2->len + 1;
//     String *dest = init_string(size + NULL_CHAR);
//
//     dest->len = size;
//     snprintf(dest->string, size + NULL_CHAR, fmt, s1->string, s2->string);
//     return dest;
// }
