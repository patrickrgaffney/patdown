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
#include <ctype.h>

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
    size_t strlength   = strlen(line);
    
    char lastScannedChar;
    
    // Return BLANK_LINE -> needed for certain block-level elements
    if (strlen(line) == 0) 
    { 
        block->blockType = BLANK_LINE;
        block->insertType = (lastBlockType == INDENTED_CODE_BLOCK) ? APPEND_NEWLINE : PLACEHOLDER;
        return block;
    }
    
    // Search the first 10 characters for a mdblock_t
    for (size_t i = 0; i < strlength; i++)
    {
        switch (line[i])
        {
            case ' ': /* INDENTED_CODE_BLOCK */
                if (lastScannedChar == ' ' && i == 3)
                {
                    *block = isIndentedCodeBlock(line, lastBlockType);
                }
                break;
            
            case '#': /* ATX_HEADING_x */
                if (lastScannedChar == '\\')
                {
                    *block = isParagraph(line, lastBlockType);
                }
                else 
                {
                    *block = isATXHeader(&line[i]);
                }
                break;
            
            case '*': /* HORIZONTAL_RULE */
                *block = isHorizontalRule(line, '*');
                break;
            
            case '-': /* SETEXT_HEADING_x */
                if (lastBlockType == PARAGRAPH)
                {
                    *block = isSetextHeading(&line[i], '-');
                }
                else
                {
                    *block = isHorizontalRule(line, '-');
                }
                break;
            
            case '_': /* HORIZONTAL_RULE */
                *block = isHorizontalRule(line, '_');
                break;
                
            case '=': /* SETEXT_HEADING_x */
                if (lastBlockType == PARAGRAPH)
                {
                    *block = isSetextHeading(&line[i], '=');
                }
                break;
            
            case '<': /* HTML_BLOCK || HTML_COMMENT */
                *block = isHTMLBlock(line, lastBlockType);
                break;
        }
        lastScannedChar = line[i];
        
        if (block->blockType != UNKNOWN) { break; }
    }
    
    // Parse a PARAGRAPH
    if ((block->blockType == PARAGRAPH && block->blockString == NULL) || block->blockType == UNKNOWN)
    {
        *block = isParagraph(line, lastBlockType);
    }
    
    return block;
}


/* ==================================================================
 * == BLOCK-LEVEL ELEMENTS
 * =============================================================== */

/* Parse for an ATX_HEADING_x. */
temp_block_node_t isATXHeader(const char *string)
{
    temp_block_node_t block;
    block.blockString = NULL;
    
    size_t level         = 0; // The level of a heading
    size_t addtSpaces    = 0; // Spaces between leading `#`s and heading
    size_t trailingChars = 0; // Spaces and `#`s after heading end (trailing `#`s)
    size_t strlength     = strlen(string);

    // If first character isnt `#`, can't be ATX_HEADING
    if (string[0] != '#')
    {
        block.blockType = UNKNOWN;
        return block;
    }
    
    // Determine the ATX_HEADING level
    for (size_t i = 0; i < 8; ++i)
    {
        if (string[i] == '#') { level++; }
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
    trailingChars = strlength;
    for (size_t i = strlength - 1; i > level + 1; --i)
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
    size_t strlength               = strlen(string);
    
    // INDENTED_CODE_BLOCK's cannot interrupt a PARAGRAPH
    if (lastBlockType == PARAGRAPH) { numAllowedLeadingSpaces = strlength; }
    
    // Remove leading spaces
    for (size_t i = 0; i < strlength; ++i)
    {
        if (string[i] == ' ') { leadingSpaces++; }
        else { atLeastOneChar = 1; break; }
    }
    
    if (leadingSpaces > numAllowedLeadingSpaces || atLeastOneChar == 0)
    {
        block.blockType   = BLANK_LINE;
        block.blockString = NULL;
        block.insertType  = PLACEHOLDER;
    }
    else
    {
        // START: leadingSpaces (remove all of them)
        // END: end of `string` (save WS for possible HARD_LINE_BREAK)
        block.blockString = allocateString(string, leadingSpaces, strlength);
        block.blockType   = PARAGRAPH;
        
        switch (lastBlockType)
        {
            case PARAGRAPH:    block.insertType = APPEND_STRING; break;
            case HTML_BLOCK:   block.insertType = APPEND_STRING; break;
            case HTML_COMMENT: block.insertType = APPEND_STRING; break;
            default: block.insertType = INSERT_NODE;
        }
    }
    return block;
}


/* Parse for an INDENTED_CODE_BLOCK. */
temp_block_node_t isIndentedCodeBlock(const char *string, const mdblock_t lastBlockType)
{
    temp_block_node_t block;
    block.blockString = NULL;
    
    // PARAGRAPHs have precedence over INDENTED_CODE_BLOCKs
    if (lastBlockType == PARAGRAPH)
    {
        block.blockType = PARAGRAPH;
        return block;
    }
    
    // First 4 characters MUST be spaces
    for (size_t i = 0; i < 3; ++i)
    {
        if (string[i] != ' ')
        {
            block.blockType   = UNKNOWN;
            return block;
        }
    }
    
    // START: 4 (always delete those metacharacters)
    // END: end of `string` (save any and all WS)
    block.blockString = allocateString(string, 4, strlen(string));
    block.blockType   = INDENTED_CODE_BLOCK;
    
    block.insertType  = (lastBlockType == INDENTED_CODE_BLOCK) ? APPEND_STRING : INSERT_NODE;
    
    return block;
}


/* Parse for a HORIZONTAL_RULE. */
temp_block_node_t isHorizontalRule(const char *string, const char character)
{
    temp_block_node_t block;
    block.insertType = INSERT_NODE;
    
    // HORIZONTAL_RULEs are given this string so it passes the != NULL test
    block.blockString = "NULL";
    
    // Number of instances of `character` encountered
    size_t numChars = 0;
    
    // Iterate over entire line, unless we find a char different from `character`
    for (size_t i = 0; i < strlen(string); ++i)
    {
        if (string[i] == ' ') { continue; }
        else if (string[i] == character) { numChars++; }
        else // anything other than WS and `character` results in UNKNOWN
        {
            block.blockType   = UNKNOWN;
            block.blockString = NULL;
            return block;
        }
    }
    
    if (numChars > 2) { block.blockType = HORIZONTAL_RULE; }
    else { block.blockType = UNKNOWN; }
    
    return block;
}


/* Parse for a SETEXT_HEADING_x. */
temp_block_node_t isSetextHeading(const char *string, const char character)
{
    temp_block_node_t block;
    
    // Number of instances of `character` encountered
    size_t numChars = 0;
    
    // Iterate over entire line, unless we find a char different from `character`
    for (size_t i = 0; i < strlen(string); ++i)
    {
        if (string[i] == character) { numChars++; }
        else if (string[i] == ' ' && numChars > 3)
        {
            block.blockType   = HORIZONTAL_RULE;
            block.blockString = "NULL";
            return block;
        }
        else // anything other than WS and `character` results in UNKNOWN
        {
            block.blockType   = UNKNOWN;
            block.blockString = NULL;
            return block;
        }
    }
    
    if (numChars >= 1)
    {
        switch (character)
        {
            case '=': block.blockType = SETEXT_HEADING_1; break;
            case '-': block.blockType = SETEXT_HEADING_2; break;
        }
        block.insertType = UPDATE_TYPE;
    }
    else { block.blockType = UNKNOWN; }
    
    return block;
}


/* Parse for an HTML_BLOCK. */
temp_block_node_t isHTMLBlock(const char *string, const mdblock_t lastBlockType)
{
    temp_block_node_t block;
    block.blockType   = UNKNOWN;
    block.blockString = NULL;
    
    size_t strlength = strlen(string);
    size_t leadingWS = 0;
    int startTag     = -1;
    int endTag       = -1;
    size_t comment   = 0;
    
    // Check to make sure there is at minimum a '<'
    // Will check for a '>', but not required.
    // Also, tag names can only contain alpha, nums, and hypens
    for (size_t i = 0; i < strlength; ++i)
    {
        if (string[i] == '<') { startTag = i; }
        // Haven't found an opening bracket yet
        else if (startTag == -1)
        {
            if (string[i] == ' ') { leadingWS++; }
            else { return block; }
        }
        // Encountered starting tag - Check for valid characters
        else if (startTag >= 0)
        {
            if (isalnum(string[i]))      { continue; }
            else if (string[i] == '!')   { comment = 1; }
            else if (string[i] == '-')   { continue; }
            else if (string[i] == ' ')   { continue; }
            else if (string[i] == '/')   { continue; }
            else if (string[i] == '>')   { endTag = i; break; }
            else { return block; }
        }
    }
    
    if (leadingWS > 3) { return block; }
    else if (comment != 0)
    {
        block.blockType = HTML_COMMENT;
    }
    else
    {
        block.blockType = HTML_BLOCK;
    }
    
    block.blockString = allocateString(string, 0, strlength);
    block.insertType  = (lastBlockType == block.blockType) ? APPEND_STRING : INSERT_NODE;
    
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