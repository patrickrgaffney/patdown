/* parsers.c
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 03/04/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file contains the implementations of functions and structures that 
 * parse lines read from the input file to determine their mdblock_t and
 * strip the string of its metacharacters and WS.
 * ======================================================================= */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parsers.h"
#include "markdown.h"
#include "utilities.h"
#include "errors.h"


/* parse_block_type(char *line, mdblock_t lastBlockType)
 * =======================================================================
 * Determines the mdblock_t of the line passed as input, then parses that
 * line to create a substring of line that is stripped of all markdown
 * characters and erroneous WS.
 *
 * Returns a pointer to a TempMarkdownBlock that contains the mdblock_t, 
 * parsed string, and insertType of line.
 * ======================================================================= */
TempMarkdownBlock *parse_block_type(char *line, mdblock_t lastBlockType)
{
    ParseInfo *parse   = NULL;
    TempMarkdownBlock *block = alloc_temp_block(NULL, BLANK_LINE, PLACEHOLDER);
    
    char *linePtr      = line;
    size_t strlength   = strlen(line);
    char c, lastScannedChar;
    
    if (strlength == 0) return block;
    
    while ((c = *linePtr++))
    {
        switch (c)
        {
            case '#': parse = parse_atx_heading(line); break;
            default: continue;
        }
        if (parse != NULL) break;
        lastScannedChar = c;
    }
    
    if (parse == NULL)
    {
        free(block);
        block = alloc_temp_block(create_substring(line, 0, strlength), PARAGRAPH, INSERT_NODE);
    }
    else 
    {
        free(block);
        block = alloc_temp_block(create_substring(line, parse->start, parse->stop), parse->type, INSERT_NODE);
        free(parse);
    }
    return block;
}


/* parse_atx_heading(char *)
 * =======================================================================
 * ATX Headings are a string between an opening sequence of indentation 
 * and pound-signs and a closing sequence of WS and pound-signs. All 
 * leading and trailing WS and pound-signs are marked for removal.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return PARAGRAPH or INDENTED_CODE_BLOCK mdblock_t in addition
 *       to ATX_HEADING_x.
 * ======================================================================= */
ParseInfo *parse_atx_heading(char *s)
{
    char c;
    size_t level        = 0; // Level of the heading.
    size_t indentation  = 0; // Amount of WS before the first `#`.
    size_t preHeadingWS = 0; // Amount of WS after the last `#`.
    
    ParseInfo *parse = alloc_parse_info(PARAGRAPH, 0, strlen(s));
    char *s2     = &s[parse->stop - 1]; // Point to last char in s.
    
    while ((c = *s++) == ' ') indentation++;
    if (indentation > 3) { parse->type = INDENTED_CODE_BLOCK; return parse; }
    
    while (c == '#') { level++; c = *s++; }
    if (level > 6) return parse;
    
    // At least one space required between last `#` and first non-WS character.
    while (c == ' ') { preHeadingWS++; c = *s++; }
    if (preHeadingWS < 1) return parse;
    
    parse->start = indentation + level + preHeadingWS;
    
    // Remove all trailing WS and `#` characters from heading.
    while ((c = *s2--) == ' ') parse->stop--;
    while (c == '#') { parse->stop--; c = *s2--; }
    while (c == ' ') { parse->stop--; c = *s2--; }
    
    switch (level)
    {
        case 1: parse->type = ATX_HEADING_1; break;
        case 2: parse->type = ATX_HEADING_2; break;
        case 3: parse->type = ATX_HEADING_3; break;
        case 4: parse->type = ATX_HEADING_4; break;
        case 5: parse->type = ATX_HEADING_5; break;
        case 6: parse->type = ATX_HEADING_6; break;
    }
    return parse;
}


/* alloc_parse_info(mdblock_t, size_t, size_t)
 * =======================================================================
 * Allocate space for a ParseInfo structure, initialize it with the 
 * mdblock_t and two sizes that were passed to the function.
 *
 * Return a pointer to the newly initializes ParseInfo structure.
 * NOTE: Function will exit program if the malloc returns NULL.
 * ======================================================================= */
ParseInfo *alloc_parse_info(mdblock_t type, size_t start, size_t stop)
{
    ParseInfo *parse = malloc(sizeof(ParseInfo));
    if (parse)
    {
        parse->type      = type;
        parse->start     = start;
        parse->stop      = stop;
        return parse;
    }
    else
    {
        atexit(print_memory_error);
        exit(EXIT_FAILURE);
    }
}