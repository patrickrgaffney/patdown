/* markdown.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    02/25/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This file contains the definitions of functions and enumerations
 * that create and iterate over a queue data structure created from
 * the block-level elements of a MD file.
 */

#ifndef MARKDOWN_H
#define MARKDOWN_H

#include <stdio.h>

/* ==================================================================
 * == ENUMERATIONS
 * =============================================================== */


/* Values for different block level MD elements. */
enum MDBlockType
{
    UNKNOWN,
    BLANK_LINE,
    HORIZONTAL_RULE,
    ATX_HEADING_1,
    ATX_HEADING_2,
    ATX_HEADING_3,
    ATX_HEADING_4,
    ATX_HEADING_5,
    ATX_HEADING_6,
    SETEXT_HEADING_1,
    SETEXT_HEADING_2,
    INDENTED_CODE_BLOCK,
    FENCED_CODE_BLOCK,
    HTML_BLOCK,
    BLOCK_COMMENT,
    LINK_REF_DEFINITION,
    PARAGRAPH,
    BLOCKQUOTE,
    UNORDERED_LIST,
    ORDERED_LIST
};

/* Synonym for `enum MDBlockType` */
typedef enum MDBlockType mdblock_t;


/* Values for different inline level MD elements. */
enum MDInlineType
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
    HARD_LINE_BREAK
};

/* Synonym for `enum MDInlineType */
typedef enum MDInlineType mdinline_t;


/* ==================================================================
 * == STRUCTURES
 * =============================================================== */

/* A type to hold one inline string of markdown text. */
struct markdownInlineNode
{
    char *inlineString;
    mdinline_t inlineType;
    struct markdownInlineNode *nextInlineNode;
};

/* Synonym for `struct markdownInlineNode` */
typedef struct markdownInlineNode inline_node_t;


/* A type to hold one block string of markdown text. This structure
 * holds nodes that will eventually be stung together to create a 
 * queue of block-level, parsed, elements.  */
struct markdownBlockNode
{
    char *blockString; 
    mdblock_t blockType;
    struct markdownBlockNode *nextBlockNode;
    struct markdownInlineNode *nextInlineNode;
};

/* Synonym for `struct markdownBlockNode` */
typedef struct markdownBlockNode block_node_t;

/* A type to temporarily hold blockStrings that are being parsed,
 * and their anticipated blockType. This structure is used as an easy
 * way to pass a group of variables (a blockString and a blockType) 
 * around between functions. When the variables are ready to be 
 * inserted into the queue, they are transered to a `block_node_t`.
 *
 * NOTE: Same as `block_node_t`; but without pointers to other nodes.
 */
struct temporaryMDBlock
{
    char *blockString;
    mdblock_t blockType;
};

/* Synonym for `struct temporaryMDBlock` */
typedef struct temporaryMDBlock temp_block_node_t;


/* ==================================================================
 * == FUNCTIONS
 * =============================================================== */

/* Parse a line of text and attempt to determine its blockType and 
 * strip the input line of all MD metacharacters.
 */
temp_block_node_t *parseBlockType(const char *line);


/* Build a linked-list of lines parsed from inputFile. */
block_node_t *buildQueue(FILE *inputFile);


/* Insert a blockNode, `newBlock` into the queue at the tail. */
void insertBlockNode(block_node_t **head, block_node_t **tail, temp_block_node_t *temp);


/* Print the queue, line-by-line, by calling `writeLine(fp, i, ...)`. */
void printQueue(block_node_t *currentNode, FILE *outputFile);


#endif