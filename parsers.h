/* 
 * parsers.h -- markdown parsing methods
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 06/15/2016.
 * 
 *********ultrapatbeams*/

#ifndef PARSERS_H
#define PARSERS_H

#include "markdown.h"

/******************************************************************
 * block_parser() -- determine which parsing function to call
 * 
 * char *line                -- line read from input file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *block_parser(char *line);


/******************************************************************
 * parse_atx_header(char *s) -- parse for an atx header
 * 
 * char *s                   -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_atx_header(char *s);


/******************************************************************
 * parse_horizontal_rule() -- parse for a horizontal rule
 * 
 * char *s                   -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_horizontal_rule(char *s);


/******************************************************************
 * parse_paragraph() -- parse for a paragraph
 * 
 * char *s                   -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_paragraph(char *s);


/******************************************************************
 * parse_setext_header() -- parse for a setext header
 * 
 * char *s                   -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_setext_header(char *s);


/******************************************************************
 * parse_indented_code_block() -- parse for an indented code block
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_indented_code_block(char *s);


/******************************************************************
 * parse_blank_line() -- parse for a blank line
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_blank_line(char *s);

#endif