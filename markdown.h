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
 * that operate on text, transforming it into markdown.
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
    BLANK_LINE,
    BLOCKQUOTE,
    UNORDERED_LIST,
    ORDERED_LIST
};

/* Synonym for `enum MDBlockType` */
typedef enum MDBlockType mdBlock;


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
typedef enum MDInlineType mdInline;


/* ==================================================================
 * == STRUCTURES
 * =============================================================== */

/* A type to hold one inline string of markdown text. */
struct markdownInlineNode
{
    char *inlineString;
    mdInline inlineType;
    struct markdownInlineNode *nextInlineNode;
};

/* Synonym for `struct markdownInlineNode` */
typedef struct markdownInlineNode inlineNode;


/* A type to hold one block string of markdown text. */
struct markdownBlockNode
{
    char *blockString;
    mdBlock blockType;
    struct markdownBlockNode *nextBlockNode;
    struct markdownInlineNode *nextInlineNode;
};

/* Synonym for `struct markdownBlockNode` */
typedef struct markdownBlockNode blockNode;


/* ==================================================================
 * == FUNCTIONS
 * =============================================================== */

/* Parse a line of text, returning the MDBlockType of that line. */
blockNode parseBlockType(const char *line);


/* Build a linked-list of lines parsed from inputFile. */
blockNode *buildList(FILE *inputFile);


/* Insert a blockNode, `newBlock` into the queue at the tail. */
void insertBlockNode(blockNode **head, blockNode **tail, blockNode newNode);


#endif