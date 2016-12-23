/**
 * strings.c -- string handling utilities
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2016-12-22
 *  project:    patdown
 * 
 ************************************************************************/

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>

#include "errors.h"
#include "strings.h"


/************************************************************************
 * # Data Array Utilities
 *
 *  A data array is defined to be an array of `uint8_t` elements. Each
 *  element of these arrays corresponds to one *byte*. An array of these
 *  bytes is used to store a UTF8 string.
 *
 ************************************************************************/

/**
 * Allocate memory for a data array of size elements.
 *
 * - parameter size: The number of elements to be stored in the array.
 *
 * - throws fatal_memory_error: Memory could not be allocated.
 *
 * - returns: A pointer to the new data array.
 */
static uint8_t *alloc_data_array(const size_t size)
{
    uint8_t *data = NULL;
    data = malloc(sizeof(uint8_t) * size);
    if (!data) throw_fatal_memory_error();
    return data;
}


/**
 * Count the leading white space in a string.
 *
 * - parameter data: An array of byte data (utf8 string).
 *
 * - returns: The number of WS characters encountered before a non-WS
 *   character, where a space is one and a tab is four.
 */
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
 * # String Nodes
 ************************************************************************/

/**
 * Allocate memory for a `String` node.
 *
 * - throws fatal_memory_error: Memory could not be allocated.
 *
 * - returns: A pointer to the new `String` node.
 */
static String *alloc_string_container(void)
{
    String *s = NULL;
    s = malloc(sizeof(String));
    if (!s) throw_fatal_memory_error();
    return s;
}


/**
 * Allocate a `String` node to store size bytes.
 *
 * If the size passed to this function is zero (0), it will allocate 
 * and return a node that has all of it's members initialized to zero.
 *
 * - parameter size: The number of bytes to allocate for the data member.
 *
 * - returns: A pointer to the new `String` node.
 */
String *init_string(const size_t size)
{
    String *str = alloc_string_container();
    
    if (size == 0) {
        str->allocd = 0;
        str->data   = NULL;
    }
    else {
        str->allocd = size;
        str->data   = alloc_data_array(size);
    }
    str->length = 0;
    return str;
}

/**
 * Reallocate a String node's data member to contain size elements.
 *
 * Often, this function is called after some parsing has begun, which
 * will inevitably increment the `data` member of `str`. Becuase of this,
 * we subtract `str` by the number of bytes currently stored in the
 * array to ensure we are reallocating on the original pointer -- 
 * which would be `str->data[0]`.
 *
 * The `str->data` array is then incremented by the same amount after
 * the reallocation -- putting it back into the state at which this
 * function was called.
 *
 * - parameter str: The String node whose member should be reallocated.
 * - parameter size: The new size (in bytes) of the requested memory.
 */
void realloc_string(String *str, const size_t size)
{
    if (str->length > 0) str->data -= str->length;
    
    str->data = realloc(str->data, sizeof(uint8_t) * size);
    str->allocd = size;
    if (!str->data) throw_fatal_memory_error();
    
    if (str->length > 0) str->data += str->length;
}


/**
 * Free the memory allocated for a String node, if it exists.
 *
 * String nodes have a dynamically allocated member, `data`
 * that is also free'd, if it exists.
 *
 * - parameter str: The String node to be free'd.
 */
void free_string(String *str)
{
    if (str) {
        if (str->data) free(str->data);
        free(str);
    }
}
