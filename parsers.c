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


/* Parse for an ATX_HEADING_x. Returns either: 
 *  1. `UNKNOWN` MDBlockType and `NULL` string
 *  2. `ATX_HEADING_x` and string with only character to be written
 *     to the outputFile.
 */
blockNode isATXHeader(const char *string)
{
    blockNode block;
    block.blockString = NULL;
    size_t level = 0;

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
    
    // Only 6 valid ATX_HEADING's
    // Only possible block element is a paragraph
    if (level > 6)
    {
        block.blockType = PARAGRAPH;
        return block;
    }
    
    // There is a required space between the last `#` and the first
    // non-`#` character. example: '##heading' is invalid
    if (string[level] != ' ')
    {
        block.blockType = PARAGRAPH;
        return block;
    }
    
    // Determine the number of trailing characters to be removed.
    // example: '## heading ##' => remove 3 chars
    size_t trailingChars = strlen(string);
    for (size_t i = strlen(string) - 1; i > level + 1; --i)
    {
        if (string[i] == ' ' || string[i] == '#')
        {
            trailingChars--;
        }
        else { break; }
    }
    
    // Allocate new string
    // START: level + 1 (add one for the required space)
    // END: remove all trailingChars
    block.blockString = allocateString(string, level + 1, trailingChars);
    
    // Return type of ATX_HEADING_level
    switch (level)
    {
        case 1:  block.blockType = ATX_HEADING_1;
            break;
        case 2:  block.blockType = ATX_HEADING_2;
            break;
        case 3:  block.blockType = ATX_HEADING_3;
            break;
        case 4:  block.blockType = ATX_HEADING_4;
            break;
        case 5:  block.blockType = ATX_HEADING_5;
            break;
        case 6:  block.blockType = ATX_HEADING_6;
            break;
        default: block.blockType = UNKNOWN;
    }
    return block;
}


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