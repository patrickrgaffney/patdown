/* 
 * parsers.h -- markdown parsing methods
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 07/11/2016.
 * 
 *********ultrapatbeams*/

#ifndef PARSERS_H
#define PARSERS_H

#include "markdown.h"

/******************************************************************
 * block_parser() -- determine which parsing function to call
 * 
 * char *line -- line read from input file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *block_parser(FILE *fp);


/******************************************************************
 * parse_paragraph() -- parse for a paragraph
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_paragraph(FILE *fp);


/******************************************************************
 * parse_setext_header() -- parse for a setext header
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_setext_header(void);


/******************************************************************
 * parse_blank_line() -- parse for a blank line
 *
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_blank_line(void);


/******************************************************************
 * parse_atx_header(char *s) -- parse for an atx header
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
// markdown_t *parse_atx_header(string_t *s);
//
//
// /******************************************************************
//  * parse_horizontal_rule() -- parse for a horizontal rule
//  *
//  * char *s -- original string read from file
//  *
//  * @return -- an markdown_t node or NULL
//  ******************************************************************/
// markdown_t *parse_horizontal_rule(string_t *s);
//
//
// /******************************************************************
//  * parse_indented_code_block() -- parse for an indented code block
//  *
//  * char *s -- original string read from file
//  *
//  * @return -- an markdown_t node or NULL
//  ******************************************************************/
// markdown_t *parse_indented_code_block(string_t *s);
//
//
// /******************************************************************
//  * parse_fenced_code_block() -- parse for a fenced code block
//  *
//  * char *s -- original string read from file
//  *
//  * @return -- an markdown_t node or NULL
//  ******************************************************************/
// markdown_t *parse_fenced_code_block(string_t *s);
//
//
// /******************************************************************
//  * parse_html_block() -- parse for an HTML block (or comment)
//  *
//  * char *s -- original string read from file
//  *
//  * @return -- an markdown_t node or NULL
//  ******************************************************************/
// markdown_t *parse_html_block(string_t *s);
//
//
// /******************************************************************
//  * parse_html_comment() -- parse for an HTML comment
//  *
//  * char *s -- original string read from file
//  *
//  * @return -- an markdown_t node or NULL
//  ******************************************************************/
// markdown_t *parse_html_comment(string_t *s);
//
#endif