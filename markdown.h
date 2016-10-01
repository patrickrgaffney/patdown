/*****
 * markdown.h -- markdown queue methods and data structures
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-30
 * 
 ************************************************************************/

#ifndef MARKDOWN_H
#define MARKDOWN_H

#include <stdbool.h>
#include <stdio.h>

#include "block_types.h"
#include "strings.h"


/************************************************************************
 * @section Markdown Data Structures
 *
 *  Markdown nodes are created and inserted into a queue. They are 
 *  distinguished by their mdblock_t and their place in the queue. The
 *  queue becomes a linear structure of nodes parsed from the file, from
 *  top to bottom.
 ************************************************************************/

/** mdblock_t -- enumerated values for a markdown_t block ***************/
typedef enum
{
    /* Meta blocks */
    UNKNOWN,                    /* Used as a placeholder. */
    BLANK_LINE,                 /* Inserted into queue as a placeholder. */
    
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
    HTML_BLOCK,                 /* Holds only content, no wrapper element. */
    HTML_COMMENT,               /* Not inserted into queue. */
    
    /* Unimplemented blocks */
    BLOCKQUOTE_START,
    BLOCKQUOTE_END,
    UNORDERED_LIST_START,
    UNORDERED_LIST_ITEM,
    UNORDERED_LIST_END,
    ORDERED_LIST_START,
    ORDERED_LIST_ITEM,
    ORDERED_LIST_END,
    LINK_REFERENCE_DEF
} mdblock_t;


/** mdinline_t -- enumerated values for a markdown_t inline span ********/
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


/** Markdown -- container node for a parsed Markdown block **************/
typedef struct Markdown
{
    string_t *value;			/* String value of parsed block. */
    mdblock_t type;				/* Type (element) of parsed block. */
    void *data;					/* (Optional) additional block data. */
    struct Markdown *next;      /* Pointer to next node in the queue. */
} Markdown;


/************************************************************************
 * @section Markdown Queue
 *
 *  These functions operate on a queue of Markdown nodes.
 ************************************************************************/

/* Allocate and initialize a new markdown_t node. */
Markdown *init_markdown(string_t *s, const size_t start, 
                        const size_t stop, const mdblock_t type);

/* Insert temp node at the tail of the markdown queue. */
void insert_markdown_queue(Markdown **head, Markdown **tail, Markdown *temp);

/* Debug-print a markdown_t node. */
void print_markdown_queue(Markdown *node);

/* Free the memory allocated for a markdown_t node. */
void free_markdown(Markdown *node);


/******************************************************************************
 * @section Specific Markdown Data Information
 *****************************************************************************/

/* Allocate space for new md_code_block_t node. */
md_code_block_t *alloc_code_block_data(void);

#endif
