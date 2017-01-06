/**
 * markdown.c -- markdown queue implementation
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2016-12-22
 *  project:    patdown
 * 
 ************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "links.h"
#include "markdown.h"
#include "strings.h"


/************************************************************************
 * # Markdown Blocks
 *
 * Markdown nodes are created and inserted into a queue. They are 
 * distinguished by their `mdblock_t` and their position in the queue. 
 * The queue forms a linear structure of nodes parsed from the file.
 *
 ************************************************************************/

/** A container node for a parsed Markdown block. */
typedef struct Markdown
{
    String *string;         /* String value of parsed block. */
    mdblock_t type;         /* Type (element) of parsed block. */
    void *addtinfo;         /* (Optional) additional block data. */
    struct Markdown *next;  /* Pointer to next node in the queue. */
} Markdown;


/** Markdown Queue Pointers */
static Markdown *head = NULL;   /* Head of the queue. */
static Markdown *tail = NULL;   /* Tail of the queue. */

/** Allow the parser to set the current block before inserting it. */
static mdblock_t currentblk = UNKNOWN;

/** Private Markdown queue functions. **/
static Markdown *md_alloc_node(void);
static bool md_insert_queue(Markdown **, Markdown **, Markdown *);
static void free_markdown_node(Markdown *);

/** Private Markdown extension functions. **/
static CodeBlk *alloc_code_blk(void);


/**
 * Allocate memory for a new Markdown block.
 *
 * - throws fatal_memory_error: Memory could not be allocated.
 *
 * - returns: A pointer to the new Markdown node.
 */
static Markdown *md_alloc_node(void)
{
    Markdown *node = NULL;
    node = malloc(sizeof(Markdown));
    if (!node) throw_fatal_memory_error();
    return node;
}


/**
 * Add a Markdown node to the queue with a given set of data.
 *
 * - parameter s: The actual string of parsed markdown.
 * - parameter type: The block type, or, HTML element.
 * - parameter addtinfo: Any additional information -- optional.
 *
 * - returns: `true` if node is inserted, `false` if node is `NULL`.
 */
bool add_markdown(String *s, const mdblock_t type, void *addtinfo)
{
    Markdown *node = md_alloc_node();
    
    /* Create an empty String node if *s was NULL. */
    if (!s) node->string = init_string(0);
    else    node->string = s;
    
    node->type     = type;
    node->addtinfo = addtinfo;
    node->next     = NULL;
    
    currentblk = UNKNOWN;
    
    if (md_insert_queue(&head, &tail, node)) return true;
    else return false;
}


/**
 * Insert a Markdown block into the queue at the tail.
 *
 * - parameter head: The first node in the queue.
 * - parameter tail: The last node in the queue.
 * - parameter node: The node to be inserted at the tail.
 * 
 * - returns: `true` if node is inserted, `false` if node is `NULL`.
 */
static bool md_insert_queue(Markdown **head, Markdown **tail, Markdown *node)
{
    if (node) {
        if (!*head) *head = node;
        else (*tail)->next = node;
        
        *tail = node;
        return true;
    }
    return false;
}


/**
 * Get the number of parsed Markdown blocks.
 *
 * - returns: The number of nodes in the queue.
 */
size_t get_queue_length(void)
{
    size_t len = 0;
    Markdown *node = head;
    
    if (!node) return 0;
    
    while (node) {
        len++;
        node = node->next;
    }
    return len;
}


/**
 * Set the current block being parsed.
 *
 * This is useful when parsing multi-line blocks (i.e. paragraphs).
 * This value will be returned from `get_last_block()` instead of
 * `tail->type` when available. The value of `currentblk` is reset to
 * `UNKNOWN` everytime `add_markdown()` is called.
 *
 * - parameter blk: The new value of `currentblk`.
 */
void set_current_block(const mdblock_t blk)
{
    currentblk = blk;
}


/**
 * Get the type of the last block added to the queue.
 *
 * - returns: The `mdblock_t` of tail, or `currentblk`, if it isn't `UNKNOWN`.
 */
mdblock_t get_last_block(void)
{
    if (currentblk != UNKNOWN) {
        return currentblk;
    }
    else if (tail) {
        return tail->type;
    }
    return UNKNOWN;
}


/**
 * Debug-print the entire Markdown queue.
 *
 *  This function is used for debugging purposes only.
 */
void debug_print_queue(void)
{
    Markdown *tmp = head;
    static char *blocknames[25] = {
        "UNKNOWN",
        "BLANK_LINE",
        "ATX_HEADER_1",
        "ATX_HEADER_2",
        "ATX_HEADER_3",
        "ATX_HEADER_4",
        "ATX_HEADER_5",
        "ATX_HEADER_6",
        "HORIZONTAL_RULE",
        "PARAGRAPH",
        "SETEXT_HEADER_1",
        "SETEXT_HEADER_2",
        "INDENTED_CODE_BLOCK",
        "FENCED_CODE_BLOCK",
        "HTML_BLOCK",
        "HTML_COMMENT",
        "LINK_REFERENCE_DEF",
        "BLOCKQUOTE_START",
        "BLOCKQUOTE_END",
        "UNORDERED_LIST_START",
        "UNORDERED_LIST_ITEM",
        "UNORDERED_LIST_END",
        "ORDERED_LIST_START",
        "ORDERED_LIST_ITEM",
        "ORDERED_LIST_END"
    };
    
    while (tmp) {
        if (tmp->type == LINK_REFERENCE_DEF) {
            printf("%s: [%s]: %s \'%s\'\n", 
                   blocknames[tmp->type],
                   ((LinkRef *)tmp->addtinfo)->label,
                   ((LinkRef *)tmp->addtinfo)->dest,
                   ((LinkRef *)tmp->addtinfo)->title);
        }
        else {
            printf("%s: \'%s\'\n", 
                   blocknames[tmp->type], 
                   tmp->string->data);
        }
        tmp = tmp->next;
    }
}


/** 
 * Free all the Markdown nodes in the queue. 
 *
 *  This is the external interface for freeing the internal
 *  Markdown queue created by parsing the input file.
 */
void free_markdown(void)
{
    free_markdown_node(head);
}


/** 
 * Free the memory allocated for a Markdown node.
 *
 *  This is the internal interface for freeing a particular
 *  Markdown node. All nodes below the node pointer to by the
 *  argument will also be free'd.
 *
 * - parameter node: The node at which to begin freeing.
 */
static void free_markdown_node(Markdown *node)
{
    if (node) {
        if (node->addtinfo) free(node->addtinfo);
        free_string(node->string);
        free_markdown_node(node->next);
        free(node);
    }
}


/************************************************************************
 * ## Markdown Block Extensions
 ************************************************************************/

/**
 * Allocate memory for a CodeBlk structure.
 *
 * - throws fatal_memory_error: Memory could not be allocated.
 *
 * - returns: A pointer to the new structure.
 */
static CodeBlk *alloc_code_blk(void)
{
    CodeBlk *cb = malloc(sizeof(CodeBlk));
    if (!cb) throw_fatal_memory_error();
    return cb;
}


/**
 * Initialize a CodeBlk structure.
 *
 * - returns: A pointer to the new structure.
 */
CodeBlk *init_code_blk(void)
{
    return alloc_code_blk();
}
