/*****
 * markdown.h -- markdown queue methods and data structures
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-10-05
 * 
 ************************************************************************/

#ifndef __MD_MARKDOWN_H__
#define __MD_MARKDOWN_H__

#include "strings.h"


/************************************************************************
 * Markdown Blocks
 *
 *  Markdown nodes are created and inserted into a queue. They are 
 *  distinguished by their mdblock_t and their place in the queue. The
 *  queue becomes a linear structure of nodes parsed from the file, from
 *  top to bottom.
 ************************************************************************/

/*****
 * Valid representations for a Markdown block.
 *
 *  These constants are used to differentiate between different types
 *  of Markdown blocks. They control how the block is written.
 *****/
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
    HTML_BLOCK,                 /* Holds only content, no wrapper element. */
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
    ORDERED_LIST_END,
} mdblock_t;


/*****
 * Valid representations for a span of Markdown->data.
 *
 *  These constants are used to differentiate between different types
 *  of text when printing Markdown->data.
 *****/
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
 * A container node for a parsed Markdown block.
 *
 *  These nodes are aligned to form a queue representing the linear
 *  structure of the input file.
 *****/
typedef struct Markdown
{
    String *string;         /* String value of parsed block. */
    mdblock_t type;         /* Type (element) of parsed block. */
    void *addtinfo;         /* (Optional) additional block data. */
    struct Markdown *next;  /* Pointer to next node in the queue. */
} Markdown;

/** Allocate a Markdown block with a String and mdblock_t. **/
Markdown *init_markdown(String *s, const mdblock_t type);


/************************************************************************
 * Markdown Queue Manipulation
 ************************************************************************/

/** Insert a Markdown block into the queue at the tail. **/
void insert_queue_tail(Markdown **head, Markdown **tail, Markdown *node);

/** Debug-print the entire Markdown queue. **/
void debug_print_queue(Markdown *node);

/** Free all the Markdown nodes in the queue. **/
void free_markdown(Markdown *node);


/************************************************************************
 * Markdown Block Extensions
 ************************************************************************/

/*****
 * A container node for additional information on a FENCED_CODE_BLOCK.
 *
 *  This node is assigned to the Markdown->addtinfo member of a 
 *  FENCED_CODE_BLOCK Markdown block.
 *****/
typedef struct
{
    char lang[20];		/* User-provided language for the code block. */
} CodeBlock;

/** Allocate memory for a new CodeBlock info node. **/
CodeBlock *alloc_code_block_info(void);

#endif
