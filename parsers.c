/* parsers.c
 *
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    03/04/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This file contains the implementations of functions that parse a 
 * string of text to determine its block level type.
 * 
 * One helper function is defined to factor out the allocation
 * of the new strings that will be created after a block level type
 * is determined.
 */

#include <stdlib.h>
#include <string.h>

#include "parsers.h"
#include "markdown.h"


/* ==================================================================
 * == MAIN PARSING FUNCTION
 * ==== All individual parsing functions are called from here.
 * =============================================================== */

/* Parse a line of text and attempt to determine its blockType. 
 * Return a tempBlock structure containing:
 *  1. The assigned blockType.
 *  2. The string containing only the characters to be written to the
 *     outputFile.
 *      - AKA: Stripped of its metacharacters.
 *
 * This is accomplished by scanning the first four characters of each
 * line for unique syntactical tokens to markdown. A function of the 
 * form `isBlockElement()` is called to determine the validity of 
 * each assumption.
 */
temp_block_node_t *parseBlockType(const char *line, mdblock_t lastBlockType)
{
    temp_block_node_t *block = malloc(sizeof(temp_block_node_t));
    block->blockType   = UNKNOWN;
    block->blockString = NULL;
    
    char lastScannedChar;
    
    // Return EMPTY_LINE -> needed for certain block-level elements
    if (strlen(line) == 0) 
    { 
        block->blockType = BLANK_LINE;
        return block;
    }
    
    // Iterate through first 4 characters of line
    for (size_t i = 0; i < 4; i++)
    {
        switch (line[i])
        {
            case ' ': // INDENTED_CODE_BLOCK
                if (lastScannedChar == ' ' && i == 3)
                    *block = isIndentedCodeBlock(line, lastBlockType);
                break;
            case '#': // ATX_HEADER 
                *block = isATXHeader(&line[i]);
                break;
        }
        lastScannedChar = line[i];
        
        if (block->blockType != UNKNOWN) { break; }
    }
    
    // Parse a PARAGRAPH
    if (block->blockType == PARAGRAPH || block->blockType == UNKNOWN)
    {
        *block = isParagraph(line, lastBlockType);
    }
    return block;
}


/* ==================================================================
 * == BLOCK-LEVEL ELEMENTS
 * =============================================================== */

/* Parse for an ATX_HEADING_x. Returns either: 
 *  1. `UNKNOWN` MDBlockType and `NULL` string
 *  2. `ATX_HEADING_x` and string with only characters to be written
 *     to the outputFile.
 */
temp_block_node_t isATXHeader(const char *string)
{
    temp_block_node_t block;
    block.blockString = NULL;
    
    size_t level         = 0; // The level of a heading
    size_t addtSpaces    = 0; // Spaces between leading `#`s and heading
    size_t trailingChars = 0; // Spaces after heading end (trailing `#`s)

    // If first character isnt `#`, can't be ATX_HEADING
    if (string[0] != '#')
    {
        block.blockType = UNKNOWN;
        return block;
    }
    
    // Determine the ATX_HEADING level
    for (size_t i = 0; i < 8; ++i)
    {
        if (string[i] == '#')
        {
            level++;
        }
        else { break; }
    }
    
    // > 6 `#`s == PARAGRAPH
    if (level > 6)
    {
        block.blockType = PARAGRAPH;
        return block;
    }
    
    // There is a required space between the last `#` and the first
    // non-`#` character. example: '##heading' is a PARAGRAPH
    if (string[level] != ' ')
    {
        block.blockType = PARAGRAPH;
        return block;
    }
    
    // Determine the number of trailing characters to be removed.
    // example: '## heading ##' => remove 3 chars
    trailingChars = strlen(string);
    for (size_t i = strlen(string) - 1; i > level + 1; --i)
    {
        if (string[i] == ' ' || string[i] == '#') { trailingChars--; }
        else { break; }
    }
    
    // Determine if the heading has been padded with extra spaces
    // after the initial set of `#`s.
    // example: '#            foo' => 'foo'
    if (string[level + 1] == ' ')
    {
        for (size_t i = level + 1; i < trailingChars; ++i)
        {
            if (string[i] == ' ') { addtSpaces++; }
            else { break; }
        }
    }
    
    // START: level + 1 (add one for the required space)
    // END: remove all trailingChars
    block.blockString = allocateString(string, level + 1 + addtSpaces, trailingChars);
    
    // Return type of ATX_HEADING_level
    switch (level)
    {
        case 1:  block.blockType = ATX_HEADING_1; break;
        case 2:  block.blockType = ATX_HEADING_2; break;
        case 3:  block.blockType = ATX_HEADING_3; break;
        case 4:  block.blockType = ATX_HEADING_4; break;
        case 5:  block.blockType = ATX_HEADING_5; break;
        case 6:  block.blockType = ATX_HEADING_6; break;
        default: block.blockType = UNKNOWN;
    }
    return block;
}


/* Parse for a PARAGRAPH. */
temp_block_node_t isParagraph(const char *string, const mdblock_t lastBlockType)
{
    temp_block_node_t block;
    
    size_t leadingSpaces           = 0; // Spaces at beginning of line
    size_t numAllowedLeadingSpaces = 3; // Otherwise, INDENTED_CODE_BLOCK
    size_t atLeastOneChar          = 0; // There is at least one character in line
    
    // INDENTED_CODE_BLOCK's cannot interrupt a PARAGRAPH
    if (lastBlockType == PARAGRAPH) { numAllowedLeadingSpaces = strlen(string); }
    
    // Remove leading spaces
    for (size_t i = 0; i < strlen(string); ++i)
    {
        if (string[i] == ' ') { leadingSpaces++; }
        else { atLeastOneChar = 1; break; }
    }
    
    if (leadingSpaces > numAllowedLeadingSpaces || atLeastOneChar == 0)
    {
        block.blockType   = BLANK_LINE;
        block.blockString = NULL;
    }
    else
    {
        // START: leadingSpaces (remove all of them)
        // END: end of `string` (save WS for possible HARD_LINE_BREAK)
        block.blockString = allocateString(string, leadingSpaces, strlen(string));
        block.blockType   = PARAGRAPH;
    }
    return block;
}


/* Parse for an INDENTED_CODE_BLOCK. */
temp_block_node_t isIndentedCodeBlock(const char *string, const mdblock_t lastBlockType)
{
    temp_block_node_t block;
    
    // First 4 characters MUST be spaces
    for (size_t i = 0; i < 3; ++i)
    {
        if (string[i] != ' ')
        {
            block.blockString = NULL;
            block.blockType   = UNKNOWN;
            return block;
        }
    }
    
    // START: 4 (always delete those metacharacters)
    // END: end of `string` (save any and all WS)
    block.blockString = allocateString(string, 4, strlen(string));
    block.blockType   = INDENTED_CODE_BLOCK;
    return block;
}


/* ==================================================================
 * == HELPER FUNCTIONS
 * =============================================================== */

/* Return a new string that contains all the characters of `string` 
 * from `string[start]` to `string[stop]`.
 */
char *allocateString(const char *string, size_t start, size_t stop)
{
    // Remove all the unneccesary characters, then add the `\0`
    size_t newstrlen = stop - start + 1;
    
    char *newstr = (char *) malloc(sizeof(char) * newstrlen);
    for (size_t i = start, j = 0; i < stop; ++i, ++j)
    {
        newstr[j] = string[i];
    }
    newstr[newstrlen - 1] = '\0';
    
    return newstr;
}


/* Combine string1 and string2 into a single string separated
 * by a newline `\n`.
 */
char *reallocateString(const char *string1, const char *string2)
{
    size_t string1len = strlen(string1);
    size_t string2len = strlen(string2);
    size_t newstrlen = string1len + string2len + 1 + 1;
    
    char *newstr = (char *) malloc(sizeof(char) * newstrlen);
    
    for (size_t i = 0; i < string1len; ++i)
    {
        newstr[i] = string1[i];
    }
    newstr[string1len] = '\n';
    
    for (size_t i = 0, j = string1len + 1; i < string2len; ++i, ++j)
    {
        newstr[j] = string2[i];
    }
    newstr[newstrlen - 1] = '\0';
    
    return newstr;
}