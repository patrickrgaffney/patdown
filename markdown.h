/*****
 * markdown.h -- markdown types and methods
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-10-13
 * 
 ************************************************************************/

#pragma once

#include "strings.h"

/** Valid types of a Markdown block. **/
typedef enum
{
    /* Meta blocks */
    UNKNOWN,                    /* Placeholder while parsing. */
    BLANK_LINE,                 /* Separates certain container blocks. */
    
    /* Implemented blocks */
    ATX_HEADER_1,               /* <h1></h1> */
    ATX_HEADER_2,               /* <h2></h2> */
    ATX_HEADER_3,               /* <h3></h3> */
    ATX_HEADER_4,               /* <h4></h4> */
    ATX_HEADER_5,               /* <h5></h5> */
    ATX_HEADER_6,               /* <h6></h6> */
    HORIZONTAL_RULE,            /* <hr> */
    PARAGRAPH,                  /* <p></p> */
    SETEXT_HEADER_1,            /* <h1></h1> */
    SETEXT_HEADER_2,            /* <h2></h2> */
    INDENTED_CODE_BLOCK,        /* <pre></pre> */
    FENCED_CODE_BLOCK,          /* <div class=""></pre> */
    HTML_BLOCK,                 /* No wrapper element. */
    HTML_COMMENT,               /* Not inserted into queue. */
    LINK_REFERENCE_DEF,         /* Not inserted into queue. */
    
    /* Unimplemented blocks */
    BLOCKQUOTE_START,
    BLOCKQUOTE_END,
    UNORDERED_LIST_START,
    UNORDERED_LIST_ITEM,
    UNORDERED_LIST_END,
    ORDERED_LIST_START,
    ORDERED_LIST_ITEM,
    ORDERED_LIST_END
} mdblock_t;


/** Valid types of a Markdown->data inline span. **/
typedef enum
{
    ESCAPED_CHAR,
    HTML_ENTITY,
    CODE_SPAN,
    EMPHASIS_SPAN,
    STRONG_SPAN,
    LINK_REFERENCE,
    IMAGE_REFERENCE,
    AUTOLINK,
    HTML_INLINE,
    LINE_BREAK
} mdinline_t;


/************************************************************************
 * Markdown Methods
 ************************************************************************/

/** Free all Markdown data. **/
void free_markdown(void);

/** Debug-print all Markdown data. **/
void debug_print_queue();

/** Add a new Markdown block to the data queue. **/
bool add_markdown(String *s, const mdblock_t type, void *addtinfo);
