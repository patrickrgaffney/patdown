/*****
 * strings.c -- string handling utilities
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-10-04
 * 
 ************************************************************************/

#ifndef __MD_STRINGS_H__
#define __MD_STRINGS_H__

#include <stddef.h>
#include <stdint.h>


/************************************************************************
 * String Nodes
 ************************************************************************/

/*****
 * A wrapper type for NULL-terminated byte strings.
 * 
 *  This type is used internally to store byte strings. It provides a 
 *  simple wrapper for bytes unsigned bytes stored in an array.
 *****/
typedef struct
{
    size_t allocd;      /* The number of bytes allocated. */
    size_t length;      /* The number of bytes written. */
    uint8_t *data;      /* The actual data. */
} String;

/** Allocate a String node to store size bytes. **/
String *init_string(const size_t size);

/** Free the memory allocated for a String node, if it exists. **/
void free_string(String *str);

void realloc_string(String *str, const size_t size);

#endif
