/**
 * markdown.h -- markdown types and methods
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2016-10-24
 *  project:    patdown
 * 
 ************************************************************************/

#pragma once

#include "strings.h"

/************************************************************************
 * Markdown Types
 ************************************************************************/

/** Valid types of a Markdown block. **/
typedef enum
{
    /* Meta blocks */
    UNKNOWN,                    /* 00. Placeholder while parsing. */
    BLANK_LINE,                 /* 01. Separates container blocks. */
    
    /* Implemented blocks */
    ATX_HEADER_1,               /* 02. <h1></h1> */
    ATX_HEADER_2,               /* 03. <h2></h2> */
    ATX_HEADER_3,               /* 04. <h3></h3> */
    ATX_HEADER_4,               /* 05. <h4></h4> */
    ATX_HEADER_5,               /* 06. <h5></h5> */
    ATX_HEADER_6,               /* 07. <h6></h6> */
    HORIZONTAL_RULE,            /* 08. <hr> */
    PARAGRAPH,                  /* 09. <p></p> */
    SETEXT_HEADER_1,            /* 10. <h1></h1> */
    SETEXT_HEADER_2,            /* 11. <h2></h2> */
    INDENTED_CODE_BLOCK,        /* 12. <pre></pre> */
    FENCED_CODE_BLOCK,          /* 13. <div class="lang"></div> */
    HTML_BLOCK,                 /* 14. No wrapper element. */
    HTML_COMMENT,               /* 15. Not inserted into queue. */
    LINK_REFERENCE_DEF,         /* 16. Not inserted into queue. */
    
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

/** Get the number of parsed Markdown blocks. **/
size_t get_queue_length(void);

/** Get the type of the last block added to the queue. **/
mdblock_t get_last_block(void);

/** Set the current block being parsed. **/
void set_current_block(const mdblock_t blk);
