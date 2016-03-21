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
    mdblock_t type; // Type of new block.
    size_t start;   // Index of first character to be copied.
    size_t stop;    // Index of last character to be copied. 
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
TempMarkdownBlock *parse_block_type(char *line, mdblock_t lastBlockType);


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
ParseInfo *parse_atx_heading(char *s);


/* alloc_parse_info(mdblock_t, size_t, size_t)
 * =======================================================================
 * Allocate space for a ParseInfo structure, initialize it with the 
 * mdblock_t and two sizes that were passed to the function.
 *
 * Return a pointer to the newly initializes ParseInfo structure.
 * NOTE: Function will exit program if the malloc returns NULL.
 * ======================================================================= */
ParseInfo *alloc_parse_info(mdblock_t type, size_t start, size_t stop);

#endif