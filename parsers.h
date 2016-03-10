/* parsers.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    03/04/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This file contains the definitions of functions that parse a 
 * string of text to determine its block level type.
 * 
 * One helper function is defined to factor out the allocation
 * of the new strings that will be created after a block level type
 * is determined.
 */

#ifndef PARSERS_H
#define PARSERS_H

#include "markdown.h"

/* ==================================================================
 * == BLOCK-LEVEL ELEMENTS
 * =============================================================== */

/* Parse for an ATX_HEADING_x. Returns either: 
 *  1. `UNKNOWN` MDBlockType and `NULL` string
 *  2. `ATX_HEADING_x` and string with only character to be written
 *     to the outputFile.
 */
blockNode isATXHeader(const char *string);




/* ==================================================================
 * == HELPER FUNCTIONS
 * =============================================================== */

/* Return a new string that contains all the characters of `string` 
 * from `string[start]` to `string[stop]`.
 */
char *allocateString(const char *string, size_t start, size_t stop);


#endif