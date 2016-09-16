/*****
 * markdown.h -- markdown queue methods and data structures
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-15
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
 ************************************************************************/

/*****
 * mdblock_t -- enumerated values for a markdown_t block
 **
 * 	Each of these values corresponds to a specific type of Markdown 
 *	block, they're used to determine how to print and/or parse the block.
 ************************************************************************/
typedef enum
{
    /* Meta blocks */
    UNKNOWN,                    // 0
    BLANK_LINE,                 // 1
    
    /* Implemented blocks */
    ATX_HEADER_1,               // 2
    ATX_HEADER_2,               // 3
    ATX_HEADER_3,               // 4
    ATX_HEADER_4,               // 5
    ATX_HEADER_5,               // 6
    ATX_HEADER_6,               // 7
    HORIZONTAL_RULE,            // 8
    PARAGRAPH,                  // 9
    SETEXT_HEADER_1,            // 10
    SETEXT_HEADER_2,            // 11
    INDENTED_CODE_BLOCK,        // 12
    FENCED_CODE_BLOCK,          // 13
    HTML_BLOCK,                 // 14
    HTML_COMMENT,               // 15
    
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


/**
 * mdinline_t -- enumerated values for a markdown_t inline span
 **
 * These values are the inline version of mdblock_t types.
 ************************************************************************/
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


/**
 * markdown_t -- container node for a parsed Markdown block
 **
 *	These nodes are constructed to form a queue -- representing the 
 *	parsed version of each raw input line (block).
 ************************************************************************/
typedef struct markdown_t
{
    string_t *value;			/* String value of parsed block.		*/
    mdblock_t type;				/* Type (element) of parsed block.		*/
    void *data;					/* (Optional) additional block data.	*/
    struct markdown_t *next;	/* Pointer to next node in the queue.	*/
} markdown_t;


/************************************************************************
 * @section Markdown Queue
 ************************************************************************/

/* Allocate / initialize a new markdown_t node. */
markdown_t *init_markdown(string_t *s, const size_t start, 
                          const size_t stop, const mdblock_t type);

/* Insert temp node at the tail of the markdown queue. */
void insert_markdown_queue(markdown_t **head, markdown_t **tail, 
                           markdown_t *temp);

/* Debug-print a markdown_t node. */
void print_markdown_queue(markdown_t *node);

/* Free the memory allocated for a markdown_t node. */
void free_markdown(markdown_t *node);


/******************************************************************************
 * @section Specific Markdown Data Information
 *****************************************************************************/

/* Allocate space for new md_code_block_t node. */
md_code_block_t *alloc_code_block_data(void);

/* Allocate space for new link_ref_t node. */
link_ref_t *alloc_link_reference_data(void);

#endif
