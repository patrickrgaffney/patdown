/*****
 * markdown.h -- markdown queue methods and data structures
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-08-29
 * 
 *****************************************************************************/

#ifndef MARKDOWN_H
#define MARKDOWN_H

#include <stdio.h>
#include <stdbool.h>

#include "strings.h"
#include "block_types.h"


/******************************************************************************
 * @section Markdown Data Structures
 *****************************************************************************/

/*****
 * Enumerated values for a `markdown_t` block.
 *
 * Each of these values corresponds to a specific type of Markdown block. 
 * These are used to determine how to print and/or parse the block.
 *****************************************************************************/
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


/*****
 * Enumerated values for a `markdown_t` inline span.
 *
 * These values are the inline version of `mdblock_t` types.
 *
 * @see mdblock_t
 *****************************************************************************/
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


/*****
 * Node containing a parsed Markdown block.
 *
 * @member  s       String (`string_t`) value of parsed markdown block.
 * @member  type    Block type of this parsed markdown block.
 * @member  data    Generic pointer to (optional) additional block data.
 * @member  next    Pointer to next node in the queue.
 *****************************************************************************/
typedef struct markdown_t
{
    string_t *value;
    mdblock_t type;
    void *data;
    struct markdown_t *next;
} markdown_t;


/******************************************************************************
 * @section Markdown Queue
 *****************************************************************************/
markdown_t *init_markdown(string_t *s, const size_t start, 
                          const size_t stop, const mdblock_t type);
void insert_markdown_queue(markdown_t **head, markdown_t **tail, 
                           markdown_t *temp);
void print_markdown_queue(markdown_t *node);
void free_markdown(markdown_t *node);


/******************************************************************************
 * @section Specific Markdown Data Information
 *****************************************************************************/
md_code_block_t *alloc_code_block_data(void);
link_ref_t *alloc_link_reference_data(void);

#endif

