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
 * == MAIN PARSING FUNCTION
 * ==== All individual parsing functions are called from here.
 * =============================================================== */

/* Parse a line of text and attempt to determine its blockType and 
 * strip the input line of all MD metacharacters.
 */
temp_block_node_t *parseBlockType(const char *line, mdblock_t lastBlockType);

/* ==================================================================
 * == BLOCK-LEVEL ELEMENTS
 * =============================================================== */

/* Parse for an ATX_HEADING_x. Returns either: 
 *  1. `UNKNOWN` mdblock_t and `NULL` string
 *  2. `ATX_HEADING_x` and string with only characters to be written
 *     to the outputFile.
 */
temp_block_node_t isATXHeader(const char *string);


/* Parse for a PARAGRAPH. */
temp_block_node_t isParagraph(const char *string, const mdblock_t lastBlockType);


/* Parse for an INDENTED_CODE_BLOCK. */
temp_block_node_t isIndentedCodeBlock(const char *string, const mdblock_t lastBlockType);


/* ==================================================================
 * == HELPER FUNCTIONS
 * =============================================================== */

/* Return a new string that contains all the characters of `string` 
 * from `string[start]` to `string[stop]`.
 */
char *allocateString(const char *string, size_t start, size_t stop);


/* Combine string1 and string2 into a single string separated
 * by a newline `\n`.
 */
char *reallocateString(const char *string1, const char *string2);

#endif