/* markdown.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 02/25/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file contains the definitions of functions, structures, and 
 * enumerations that create, maintain, and print a queue of MarkdownBlock
 * nodes that serve as the main data structure through which the program
 * flows.
 * ======================================================================= */

#ifndef MARKDOWN_H
#define MARKDOWN_H

#include <stdio.h>


/* mdblock_t
 * =======================================================================
 * Valid values for block-level Markdown elements. These values are used
 * to determine how to parse the string and how to write the parsed string
 * to the output file.
 * ======================================================================= */
typedef enum MDBlockType
{
    UNKNOWN,    /* For values that need more parsing. */
    BLANK_LINE, /* For lines that serve as a break between mdblock_t's. */
    
    /* IMPLEMENTED */
    ATX_HEADING_1,
    ATX_HEADING_2,
    ATX_HEADING_3,
    ATX_HEADING_4,
    ATX_HEADING_5,
    ATX_HEADING_6,
    
    /* UNIMPLEMENTED */
    INDENTED_CODE_BLOCK,
    PARAGRAPH,
    SETEXT_HEADING_1,
    SETEXT_HEADING_2,
    HTML_BLOCK,
    HTML_COMMENT,
    FENCED_CODE_BLOCK_START,
    FENCED_CODE_BLOCK,
    FENCED_CODE_BLOCK_END,
    HORIZONTAL_RULE,
    BLOCK_COMMENT,
    LINK_REF_DEFINITION,
    BLOCKQUOTE_START,
    BLOCKQUOTE_TEXT,
    BLOCKQUOTE_END,
    UNORDERED_LIST_START,
    UNORDERED_LIST_ITEM,
    UNORDERED_LIST_END,
    ORDERED_LIST_START,
    ORDERED_LIST_ITEM,
    ORDERED_LIST_END
} mdblock_t; 


/* mdinline_t
 * =======================================================================
 * Valid values for inline-level Markdown elements. These values are used 
 * to determine how to parse the inline strings, and how they are written
 * to the output file.
 * ======================================================================= */
typedef enum MDInlineType
{
    UNKNOWN_INLINE,
    ESCAPED_CHAR,
    ENTITY_REFERENCE,
    NUMERIC_CHAR_REFERENCE,
    HEX_CHAR_REFERENCE,
    CODE_SPAN,
    ITALICS_EMPHASIS,
    BOLD_EMPHASIS,
    LINK_TEXT,
    LINK_URL,
    IMAGE_LINK_TEXT,
    IMAGE_LINK_URL,
    AUTO_LINK,
    RAW_HTML,
    HARD_LINE_BREAK,
    FENCED_CODE_LANGUAGE
} mdinline_t;


/* insert_t
 * =======================================================================
 * Values used to determine if and how a MarkdownBlock node is inserted
 * into the queue. All actions are applied to the tail node of the queue.
 * ======================================================================= */
typedef enum MDInsertType
{
    APPEND_STRING,  /* Append the current node's string to that of the tailNode. */
    PLACEHOLDER,    /* This is a placeholder node, they do not get inserted. */
    INSERT_NODE,    /* Insert this node as a new node at tail of queue. */
    APPEND_NEWLINE, /* Append a newline to the tailNode's string. */
    UPDATE_TYPE     /* Update the previous node's blockType to the currentNodes. */
} insert_t;


/* MarkdownInline
 * =======================================================================
 * Type to hold information about a parsed string of inline markdown. Each
 * node has its own mdinline_t, and the inlineString is parsed accordingly.
 * ======================================================================= */
typedef struct markdownInlineNode
{
    char *inlineString;
    mdinline_t inlineType;
    struct markdownInlineNode *nextInlineNode;
} MarkdownInline;


/* MarkdownBlock
 * =======================================================================
 * Type to hold information about a block of markdown. Individual nodes of
 * MarkdownBlock will make up the nodes of the queue. MarkdownInline nodes
 * are enqueued at the nextInlineNode pointer, creating a 2D queue.
 * ======================================================================= */
typedef struct markdownBlockNode
{
    char *blockString; 
    mdblock_t blockType;
    MarkdownInline *nextInlineNode;
    struct markdownBlockNode *nextBlockNode;
} MarkdownBlock;


/* TempMarkdownBlock
 * =======================================================================
 * Type to hold information about a block of markdown *while* it is being
 * parsed. The values held in a TempMarkdownBlock node are subject to be 
 * changed or updated at any time before they are transferred to a 
 * MarkdownBlock node and inserted into the queue.  This type is a 
 * convenient way to pass a group of values around the different parsing
 * functions.
 * ======================================================================= */
typedef struct temporaryMDBlock
{
    char *blockString;
    mdblock_t blockType;
    insert_t insertType;
} TempMarkdownBlock;


/* buildQueue(FILE *)
 * =======================================================================
 * Build a queue of parsed lines, each read sequentially from the input
 * file. This function calles parse_block_type() and the individual 
 * parsers are called from there.
 *
 * Returns a pointer to the head of the queue.
 * ======================================================================= */
MarkdownBlock *build_queue(FILE *inputFile);


/* insert_block_node(MarkdownBlock, MarkdownBlock, TempMarkdownBlock)
 * =======================================================================
 * Create a new MarkdownBlock node, using the information obtained from 
 * parsing in the TempMarkdownBlock. Insert the new node at the tail of
 * the queue; unless **head is NULL, then the **tail is the **head.
 * ======================================================================= */
void insert_block_node(MarkdownBlock **head, MarkdownBlock **tail, TempMarkdownBlock *temp);


/* print_queue(MarkdownBlock, FILE *)
 * =======================================================================
 * Prints all the blockString and all the inlineString's at currentNode to
 * the output file. Then makes a recursive call to accomplish the same
 * task at the node pointed to by nextBlockNode. Printing is accomplished
 * by making calls to the write_line(FILE *, i, ...) function.
 * ======================================================================= */
void print_queue(MarkdownBlock *currentNode, FILE *outputFile);


/* alloc_block(char *, mdblock_t)
 * =======================================================================
 * Allocate space for a MarkdownBlock structure, initialize it with the 
 * string and mdblock_t that were passed to the function.
 *
 * Return a pointer to the newly initialized MarkdownBlock structure.
 * NOTE: Function will exit program if the malloc returns NULL.
 * ======================================================================= */
MarkdownBlock *alloc_block(char *s, mdblock_t type);


/* alloc_temp_block(char *, mdblock_t, insert_t)
 * =======================================================================
 * Allocate space for a TempMarkdownBlock structure, initialize it with 
 * the string, mdblock_t, and insert_t that were passed to the function.
 *
 * Return a pointer to the newly initialized TempMarkdownBlock structure.
 * ======================================================================= */
TempMarkdownBlock *alloc_temp_block(char *s, mdblock_t type, insert_t insert);


/* free_block(MarkdownBlock)
 * =======================================================================
 * Free the space at the MarkdownBlock (if it is not NULL) in the 
 * following order:
 *      1. Free blockString (if not NULL).
 *      2. Free space at nextInlineNode by calling freeInlineQueue().
 *      3. Free space at nextBlockNode by making a recursive call.
 *      4. Free space at currentNode.
 * ======================================================================= */
void free_block(MarkdownBlock *currentNode);


/* free_inline_queue(MarkdownInline)
 * =======================================================================
 * Free the space at the MarkdownInline structure (if it is not NULL) in 
 * the following order:
 *      1. Free inlineString (if not NULL).
 *      2. Free space at nextInlineNode by making a recursive call.
 *      3. Free space at currentNode.
 * ======================================================================= */
void free_inline_queue(MarkdownInline *currentNode);

#endif