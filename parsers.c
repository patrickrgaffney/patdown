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


/* parse_block_type(char *, mdblock_t)
 * =======================================================================
 * This is the grand central station of block-level parsing. Given a line
 * from input, determine its mdblock_t and parse it to remove all WS and
 * metacharacters.
 *
 * Returns a TempMarkdownBlock which contains the mdblock_t, parsed 
 * string, and insert_t of the input string. The TempMarkdownBlock now 
 * contains all the necessary information to be added to the stack.
 * ======================================================================= */
TempMarkdownBlock *parse_block_type(char *line, mdblock_t last)
{
    ParseInfo *parse         = NULL;
    TempMarkdownBlock *block = NULL;
    char *linePtr = line;
    char c, lastChar, *newstr;
    
    if (strlen(line) == 0) return alloc_temp_block(NULL, BLANK_LINE, PLACEHOLDER);
    
    while ((c = *linePtr++))
    {
        switch (c)
        {
            case ' ': parse = parse_indented_code_block(line, last); break;
            case '#': parse = parse_atx_heading(line, last); break;
            case '*': parse = parse_horizontal_rule(line, '*', last); break;
            case '_': parse = parse_horizontal_rule(line, '_', last); break;
            case '-': parse = parse_horizontal_rule(line, '-', last); break;
            case '=': parse = parse_setext_heading(line, '=', last); break;
            default: break;
        }
        if (parse != NULL) break;
        lastChar = c;
    }
    
    // Default to a PARAGRAPH
    if (parse == NULL) parse = parse_paragraph(line, last);
    
    // PLACEHOLDERs are not enqueued, therefore string == NULL
    if (parse->insert == PLACEHOLDER)
    {
       block = alloc_temp_block(NULL, BLANK_LINE, PLACEHOLDER);
    }
    else
    {
        newstr = create_substr(line, parse->start, parse->stop);
        if (newstr) block = alloc_temp_block(newstr, parse->type, parse->insert);
        else        block = alloc_temp_block(line, parse->type, parse->insert);
    }
    
    free(parse);
    return block;
}


/* parse_atx_heading(char *)
 * =======================================================================
 * ATX Headings are a string between an opening sequence of indentation 
 * and pound-signs and a closing sequence of WS and pound-signs. All 
 * leading and trailing WS and pound-signs are marked for removal.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return a PARAGRAPH or an INDENTED_CODE_BLOCK.
 * ======================================================================= */
ParseInfo *parse_atx_heading(char *s, mdblock_t last)
{
    char c;
    size_t level        = 0; // Level of the heading.
    size_t indentation  = 0; // Amount of WS before the first `#`.
    size_t preHeadingWS = 0; // Amount of WS after the last `#`.
    char *sPtr          = s; // Point to the first char in s.
    size_t strlength    = strlen(s);
    char *sPtr2         = &s[strlength - 1]; // Point to last char in s.
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation > 3) return parse_indented_code_block(s, last);
    
    while (c == '#') { level++; c = *sPtr++; }
    if (level > 6) return parse_paragraph(s, last);
    
    // At least one space required between last `#` and first non-WS character.
    while (c == ' ') { preHeadingWS++; c = *sPtr++; }
    if (preHeadingWS < 1) return parse_paragraph(s, last);
    
    // Remove all trailing WS and `#` characters from heading.
    while ((c = *sPtr2--) == ' ') strlength--;
    while (c == '#') { strlength--; c = *sPtr2--; }
    while (c == ' ') { strlength--; c = *sPtr2--; }
    
    switch (level)
    {
        case 1:  return alloc_parse_info(ATX_HEADING_1, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        case 2:  return alloc_parse_info(ATX_HEADING_2, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        case 3:  return alloc_parse_info(ATX_HEADING_3, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        case 4:  return alloc_parse_info(ATX_HEADING_4, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        case 5:  return alloc_parse_info(ATX_HEADING_5, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        case 6:  return alloc_parse_info(ATX_HEADING_6, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        default: return parse_paragraph(s, last);
    }
}

/* parse_paragraph(char *, mdblock_t)
 * =======================================================================
 * Paragraphs are a sequence of lines containing at least one non-WS
 * character. They are used as the "fall-back mdblock_t"; in other words,
 * they are the last resort when the mdblock_t cannot be determined. 
 * Because of this, there is not a lot of parsing to be done, it is likely
 * that all other options have been exhausted by the time this function is
 * called. All we do is remove the leading WS.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return an INDENTED_CODE_BLOCK.
 * ======================================================================= */
ParseInfo *parse_paragraph(char *s, mdblock_t last)
{
    char c;
    char *sPtr = s;          // Point to the first character in s.
    size_t indentation  = 0; // Amount of WS before the first non-WS char.
    size_t maxLeadingWS = 3; // Max number of valid leading WS chars.
    
    // If we are appending this line, the indentation can be unlimited.
    if (last == PARAGRAPH) maxLeadingWS = 100;
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation > maxLeadingWS) return parse_indented_code_block(s, last);
    
    if (last == PARAGRAPH)
    {
        return alloc_parse_info(PARAGRAPH, indentation, strlen(s), APPEND_STRING);
    }
    else return alloc_parse_info(PARAGRAPH, indentation, strlen(s), INSERT_NODE);
}


/* parse_indented_code_block(char *, mdblock_t)
 * =======================================================================
 * Indented code blocks are composed of lines indented with at least four
 * leading WS characters. Any less than 4, and the block cannot be an 
 * indented code block. Lines with only WS are indented code blocks if and
 * only if they have at least 4 characters of WS indentation, otherwise 
 * they are BLANK_LINEs.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return a PARAGRAPH or NULL.
 * ======================================================================= */
ParseInfo *parse_indented_code_block(char *s, mdblock_t last)
{
    // INDENTED_CODE_BLOCKs cannot interrupt PARAGRAPHs.
    if (last == PARAGRAPH) return NULL;
    
    char c;
    char *sPtr = s;         // Point to the first char in s.
    size_t indentation = 0; // Amount of WS before the fist non-WS char.
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation < 4 && c == '\0') return alloc_parse_info(BLANK_LINE, 0, 1, PLACEHOLDER);
    else if (indentation < 4) return NULL;
    
    if (last == INDENTED_CODE_BLOCK)
    {
        return alloc_parse_info(INDENTED_CODE_BLOCK, 4, strlen(s), APPEND_STRING);
    }
    else return alloc_parse_info(INDENTED_CODE_BLOCK, 4, strlen(s), INSERT_NODE);
}


/* parse_horizontal_rule(char *, char, mdblock_t)
 * =======================================================================
 * Horizontal rules are lines consisting of three or more matching `-`, 
 * `_`, or `*` characters, each followed by an unlimited number of WS. 
 * Lines containing less than 3 of these marks are parsed as paragraphs.
 * These lines cannot contain any other characters than the chosen mark
 * and WS, if any other character is encountered, the line is parsed as a
 * paragraph.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return a PARAGRAPH, INDENTED_CODE_BLOCK, or SETEXT_HEADING_2.
 * ======================================================================= */
ParseInfo *parse_horizontal_rule(char *s, char mark, mdblock_t last)
{
    // If any ambiguity choose SETEXT_HEADING_2
    if (last == PARAGRAPH && mark == '-') return parse_setext_heading(s, mark, last);
    
    char c;
    char *sPtr = s;         // Point to the first char in s.
    size_t indentation = 0; // Amount of WS before the first non-WS char.
    size_t numberMarks = 0; // Number of *marked* characters encountered.
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation > 3) return parse_indented_code_block(s, last);
    
    // Unlimited number of marks separated by unlimited spaces
    while (c == mark || c == ' ') 
    {
        if (c == mark) numberMarks++;
        c = *sPtr++;
    }
    
    // If we encountered any character other than mark or WS, return PARAGRAPH
    if (c != '\0') return parse_paragraph(s, last);

    if (numberMarks < 3) return parse_paragraph(s, last);
    
    return alloc_parse_info(HORIZONTAL_RULE, 0, 1, INSERT_NODE);
}


/* parse_setext_heading(char *, char, mdblock_t)
 * =======================================================================
 * Setext headings are a line consisting of a single character, similar to
 * horizontal rules, in the block immediately following a paragraph. They
 * serve the upgrade the previous block from a paragraph to a header of 
 * level 1 or 2.
 *
 * NOTE: When the mark is '-', parse_block_type() will call the horizontal
 *       rule parser first, and it will call this parser.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return NULL, INDENTED_CODE_BLOCK, or PARAGRAPH.
 * ======================================================================= */
ParseInfo *parse_setext_heading(char *s, char mark, mdblock_t last)
{
    // Last block be a PARAGRAPH to be parsed as a SETEXT_HEADING_x
    if (last != PARAGRAPH) return NULL;
    
    char c;
    char *sPtr = s;         // Point to the first char in s.
    size_t indentation = 0; // Amount of WS before the first non-WS char.
    size_t numberMarks = 0; // Number of *marked* characters encountered.
    size_t trailingWS  = 0; // Amount of WS after the last *mark*.
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation > 3) return parse_indented_code_block(s, last);
    
    while (c == mark) { numberMarks++; c = *sPtr++; }
    if (numberMarks < 1) return NULL;
    
    while (c == ' ') { trailingWS++; c = *sPtr++; }
    
    if (c != '\0') return parse_paragraph(s, last);
    
    switch (mark)
    {
        case '=': return alloc_parse_info(SETEXT_HEADING_1, 0, 1, UPDATE_TYPE);
        case '-': return alloc_parse_info(SETEXT_HEADING_2, 0, 1, UPDATE_TYPE);
        default:  return parse_paragraph(s, last);
    }
}


/* alloc_parse_info(mdblock_t, size_t, size_t)
 * =======================================================================
 * Allocate space for a ParseInfo structure, initialize it with the 
 * mdblock_t and two sizes that were passed to the function.
 *
 * Return a pointer to the newly initializes ParseInfo structure.
 * NOTE: Function will exit program if the malloc call returns NULL.
 * ======================================================================= */
ParseInfo *alloc_parse_info(mdblock_t type, size_t start, size_t stop, insert_t insert)
{
    ParseInfo *parse = malloc(sizeof(ParseInfo));
    if (parse)
    {
        parse->type   = type;
        parse->start  = start;
        parse->stop   = stop;
        parse->insert = insert;
        return parse;
    }
    else
    {
        atexit(print_memory_error);
        exit(EXIT_FAILURE);
    }
}