/* 
 * markdown.h -- markdown structures and methods.
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 07/21/2016.
 * 
 *********ultrapatbeams*/

#ifndef MARKDOWN_H
#define MARKDOWN_H

#include <stdio.h>
#include <stdbool.h>
#include "strings.h"


/******************************************************************
 * mdblock_t -- enumerated values for a markdown_t block
 ******************************************************************/
typedef enum
{
    ///// META
    UNKNOWN,                    // 0
    BLANK_LINE,                 // 1
    
    ///// IMPLEMENTED
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
    FENCED_CODE_BLOCK_START,    // 13
    FENCED_CODE_BLOCK,          // 14
    FENCED_CODE_BLOCK_END,      // 15
    
    ///// UNIMPLEMENTED
    BLOCKQUOTE_START,
    BLOCKQUOTE_END,
    UNORDERED_LIST_START,
    UNORDERED_LIST_ITEM,
    UNORDERED_LIST_END,
    ORDERED_LIST_START,
    ORDERED_LIST_ITEM,
    ORDERED_LIST_END,
    LINK_REFERENCE_DEF,
    HTML_BLOCK
} mdblock_t;


/******************************************************************
 * mdinline_t -- enumerated values for a markdown_t inline span
 ******************************************************************/
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


/******************************************************************
 * markdown_t -- a node containing a parsed markdown block
 *
 * char *s         -- string value of parsed markdown block
 * mdblock_t type  -- block type of this parsed markdown block
 * markdown_t next -- pointer to the next markdown_t node in stack
 ******************************************************************/
typedef struct markdown_t
{
    string_t *value;
    mdblock_t type;
    struct markdown_t *next;
} markdown_t;


/******************************************************************
 * markdown() -- convert a markdown file to an output type
 *
 * FILE *inputFile -- markdown file, opened for reading
 *
 * @return -- the head of a markdown_t stack
 ******************************************************************/
markdown_t *markdown(FILE *inputFile);


/******************************************************************
 * init_markdown() -- create a new markdown_t node
 * 
 * char *s              -- original string block read from file
 * const size_t start   -- index where the string should start
 * const size_t stop    -- index where the string should end
 * const mdblock_t type -- mdblock_t of new markdown_t node
 *
 * @return -- an initialized markdown_t node
 ******************************************************************/
markdown_t *init_markdown(string_t *s, const size_t start, 
                          const size_t stop, const mdblock_t type);


/******************************************************************
 * alloc_markdown() -- allocate space for a new markdown_t node
 *
 * @throws -- throw_memory_error() if node cannot be allocated
 * @return -- an allocated markdown_t node
 ******************************************************************/
markdown_t *alloc_markdown(void);


/******************************************************************
 * insert_markdown_queue() -- place a new node at the queue's tail
 *
 * markdown_t **head -- current head of the queue
 * markdown_t **tail -- current tail of the queue
 * markdown_t  *temp -- node to be inserted
 *
 * @noreturn  -- control returned to the caller
 ******************************************************************/
void insert_markdown_queue(markdown_t **head, markdown_t **tail, markdown_t *temp);


/******************************************************************
 * print_markdown_queue() -- print the queue to stdout
 *
 * markdown_t *node -- node to be printed
 *
 * @noreturn  -- control returned to the caller
 ******************************************************************/
void print_markdown_queue(markdown_t *node);


/******************************************************************
 * alloc_markdown() -- free markdown_t node, if it exists
 *
 * markdown_t *node -- node to be freed
 *
 * @recursive -- calls node->next for every node
 ******************************************************************/
void free_markdown(markdown_t *node);


/******************************************************************
 * update_queue() -- update the last node based on new information
 *
 * markdown_t **tail -- node at tail of queue
 * markdown_t  *temp -- (new) node that was most recently parsed
 *
 * @return -- true if queue was updated, false if not
 ******************************************************************/
bool update_queue(markdown_t **tail, markdown_t *temp);


#endif