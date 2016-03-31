/* parsers.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 03/04/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file contains the definitions of functions and structures that 
 * parse lines read from the input file to determine their mdblock_t and
 * strip the string of its metacharacters and WS.
 * ======================================================================= */

#ifndef PARSERS_H
#define PARSERS_H

#include "markdown.h"


/* ParseInfo
 * =======================================================================
 * Return value for each of the block-level parsing functions. Contains 
 * information needed to make a substring out of the line read from the 
 * input file, stripped of all its metacharacters and WS.
 * ======================================================================= */
typedef struct parsingInformation
{
    mdblock_t type;  // Type of new block.
    size_t start;    // Index of first character to be copied.
    size_t stop;     // Index of last character to be copied.
    insert_t insert; // Type of enqueue to perform.
} ParseInfo;


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
TempMarkdownBlock *parse_block_type(char *line);


/* parse_atx_heading(char *)
 * =======================================================================
 * ATX Headings are a string between an opening sequence of indentation 
 * and pound-signs and a closing sequence of WS and pound-signs. All 
 * leading and trailing WS and pound-signs are marked for removal.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return a PARAGRAPH or an INDENTED_CODE_BLOCK.
 * ======================================================================= */
ParseInfo *parse_atx_heading(char *s);


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
ParseInfo *parse_paragraph(char *s);


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
ParseInfo *parse_indented_code_block(char *s);


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
 * NOTE: Can return a PARAGRAPH or INDENTED_CODE_BLOCK.
 * ======================================================================= */
ParseInfo *parse_horizontal_rule(char *s, char mark);


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
ParseInfo *parse_setext_heading(char *s, char mark);


/* parse_code_fence(char *, char, mdblock_t)
 * =======================================================================
 * The opening (FENCED_CODE_BLOCK_START) and closing 
 * (FENCED_CODE_BLOCK_STOP) fences are the only thing parsed in this 
 * function. But because we can **guarantee** that this will be the first
 * parsing function called if the last container block was either a
 * FENCED_CODE_BLOCK or a FENCED_CODE_BLOCK_START, we return 
 * FENCED_CODE_BLOCK if we can determine that we are **in** a fenced code
 * block container and the current line is **not** the closing fence.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return INDENTED_CODE_BLOCK, FENCED_CODE_BLOCK, and NULL.
 * ======================================================================= */
ParseInfo *parse_code_fence(char *s, char mark);


/* parse_html_block(char *,)
 * =======================================================================
 * HTML blocks start with a line where the first character is a left-angle
 * bracket, `<`.  Every consecutive line will be appended to this 
 * HTML_BLOCK mdblock_t until we encounter a newline, in which the 
 * container will end.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return INDENTED_CODE_BLOCK and BLANK_LINE
 * ======================================================================= */
ParseInfo *parse_html_block(char *s);


ParseInfo *parse_link_definition(char *s);


#endif