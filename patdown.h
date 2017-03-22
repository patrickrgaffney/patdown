/**
 * patdown.h -- markdown types and methods
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2017-01-06
 *  project:    patdown
 * 
 ************************************************************************/

#ifndef PATDOWN_DOT_H
#define PATDOWN_DOT_H

#include <stdbool.h>

#include "strings.h"

/************************************************************************
 * # Markdown Types
 ************************************************************************/

/** Valid types of a Markdown block. */
typedef enum
{
    /* Meta blocks */
    UNKNOWN,                    /* 00. Placeholder while parsing. */
    
    /* Implemented blocks */
    BLANK_LINE,                 /* 01. Separates container blocks. */
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
    HTML_BLOCK,                 /* 14. Raw HTML. */
    HTML_COMMENT,               /* 15. Raw HTML. */
    LINK_REFERENCE_DEF,         /* 16. Inserted into queue for testing. */
    BLOCKQUOTE_START,           /* 17. <blockquote> */
    BLOCKQUOTE_END,             /* 18. </blockquote> */
    
    /* Unimplemented blocks */
    UNORDERED_LIST_START,
    UNORDERED_LIST_END,
    ORDERED_LIST_START,
    ORDERED_LIST_END,
    LIST_ITEM_START,
    LIST_ITEM_END,
} mdblock_t;


/** Valid types of a Markdown inline span. */
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
 * # Markdown Methods
 ************************************************************************/

/** Free all Markdown data. */
void free_markdown(void);

/** Debug-print all Markdown data. */
void debug_print_queue(void);

/** Add a new Markdown block to the data queue. */
bool add_markdown(String *, const mdblock_t, void *);

/** Get the number of parsed Markdown blocks. */
size_t get_queue_length(void);

/** Get the type of the last block added to the queue. */
mdblock_t get_last_block(void);

/** Dequeue the last block added to the queue. */
String *dequeue_last_block(void);

/** Set the current block being parsed. */
void set_current_block(const mdblock_t);


/************************************************************************
 * # Markdown Block Extensions
 *
 * A block extension is a set of additional information about a specific
 * block that is saved during parsing. All of the block extensions are
 * static in memory -- in other words, their memory is managed by
 * calling `free_markdown()`.
 *
 ************************************************************************/

/************************************************************************
 * ##  Fenced Code Blocks Extension
 ************************************************************************/

/** Maximum length of an info string on a fenced code block. */
#define INFO_STR_MAX 20

/**
 * A type to hold additional information about a fenced code block.
 *
 * The members of this type are used to parse and write the fenced
 * code block as output.
 *
 * - member lang: Info string on the opening fence.
 * - member ws: Indentation on the opening fence.
 * - member fl: Length of the opening code fence.
 * - member fc: Code fence character `\`` or `~`.
 */
typedef struct CodeBlk
{
    uint8_t lang[INFO_STR_MAX]; /* Info string on the opening fence. */
    size_t ws;                  /* Indentation on the opening fence. */
    size_t fl;                  /* Length of the opening code fence. */
    int8_t fc;                  /* Code fence character (`|~). */
} CodeBlk;


/** Allocate a CodeBlk structure. **/
CodeBlk *init_code_blk(void);


/************************************************************************
 * # Markdown Parsing Functions
 ************************************************************************/

/** Call upon the parsers and generate the Markdown queue. */
bool markdown(String *rawBytes);


/************************************************************************
 * # Markdown Output Types
 ************************************************************************/

/** Valid ouput type constants. */
typedef enum 
{
    OUT_HTML5,      /* Default: HTML5 element syntax. */ 
    OUT_PARSED      /* Internal parsing information (for debugging). */
} output_t;


/************************************************************************
 * # Link Reference Type
 *
 * A `LinkRef` object is a node in the binary search tree that's 
 * internal to the links.c file. This type is exposed so that links can 
 * be inserted into the `Markdown` queue. This is useful for testing and 
 * semantic analysis of other links / the markdown file itself.
 *
 ************************************************************************/

/**
 * A type to hold link data information. 
 * 
 * There are three important pieces of a link: a label, destination, and
 * title. Each are stored as static arrays of 1000 `char` elements.
 *
 * - member label: the link label -- unique identifier for every link.
 * - member dest: the link destination -- the URL to link to.
 * - member title: the link title -- an optional `title` attribute.
 * - member left: a pointer to the left subtree relative to self.
 * - member right: a pointer to the right subtree relative to self.
 */
typedef struct LinkRef
{
    char label[1000];
    char dest[1000];
    char title[1000];
    struct LinkRef *left;
    struct LinkRef *right;
} LinkRef;


/************************************************************************
 * # Link Reference Methods
 *
 * These methods create a binary search tree of `LinkRef` nodes that can
 * then be searched to resolve inline link references.
 *
 ************************************************************************/

/** Allocate space for new `LinkRef` node. */
LinkRef *init_link_ref(void);

/** Add a `LinkRef` node the the internal binary search tree. */
// void add_link_ref(LinkRef *node);

/** Search the binary tree for a particular link label. */
// LinkRef *search_link_refs(char *label);

/** Free all nodes in the private binary search tree. */
void free_link_refs(void);

#endif
