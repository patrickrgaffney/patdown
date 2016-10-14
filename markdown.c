/*****
 * markdown.c -- markdown queue implementation
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2016-10-13
 *  project:    patdown
 * 
 ************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "markdown.h"
#include "strings.h"


/************************************************************************
 * Markdown Blocks
 *
 *  Markdown nodes are created and inserted into a queue. They are 
 *  distinguished by their mdblock_t and their place in the queue. The
 *  queue becomes a linear structure of nodes parsed from the file, from
 *  top to bottom.
 ************************************************************************/

/** A container node for a parsed Markdown block. **/
typedef struct Markdown
{
    String *string;         /* String value of parsed block. */
    mdblock_t type;         /* Type (element) of parsed block. */
    void *addtinfo;         /* (Optional) additional block data. */
    struct Markdown *next;  /* Pointer to next node in the queue. */
} Markdown;


/** Markdown Queue Pointers **/
static Markdown *head = NULL;   /* Head of the queue. */
static Markdown *tail = NULL;   /* Tail of the queue. */


/** File private functions. **/
static Markdown *md_alloc_node(void);
static bool md_insert_queue(Markdown **head, Markdown **tail, Markdown *node);


/*****
 * Allocate memory for a new Markdown block.
 *
 * ERRORS
 *  fatal_memory_error  Memory could not be allocated.
 *
 * RETURNS
 *  A pointer to the new Markdown node.
 *****/
static Markdown *md_alloc_node(void)
{
    Markdown *node = NULL;
    node = malloc(sizeof(Markdown));
    if (!node) throw_fatal_memory_error();
    return node;
}


/*****
 * Add a Markdown node to the queue with a given set of data.
 *
 * ARGUMENTS
 *  s           The actual string of parsed markdown.
 *  type        The block type, or, HTML element.
 *  addtinfo    Any additional information -- optional.
 *
 * RETURNS
 *  true if node is inserted, false if node is NULL.
 *****/
bool add_markdown(String *s, const mdblock_t type, void *addtinfo)
{
    Markdown *node = md_alloc_node();
    node->string   = s;
    node->type     = type;
    node->addtinfo = addtinfo;
    node->next     = NULL;
    
    if (md_insert_queue(&head, &tail, node)) return true;
    else return false;
}


/*****
 * Insert a Markdown block into the queue at the tail.
 *
 * ARGUMENTS
 *  head    The first node in the queue.
 *  tail    The last node in the queue.
 *  node    The node to be inserted at the tail.
 * 
 * RETURNS
 *  true if node is inserted, false if node is NULL.
 *****/
static bool md_insert_queue(Markdown **head, Markdown **tail, Markdown *node)
{
    if (node) {
        if (!*head) *head = node;
        else (*tail)->next = node;
        *tail = node;
        return true;
    }
    else return false;
}


/*****
 * Debug-print the entire Markdown queue.
 *
 *  This function is used for debugging purposes only.
 *****/
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
        printf("%s: \'%s\'\n", blocknames[tmp->type], tmp->string->data);
        tmp = tmp->next;
    }
}


/*****
 * Free all the Markdown nodes in the queue.
 *****/
void free_markdown(void)
{
    if ((tail = head)) {
        if (tail->addtinfo) free(tail->addtinfo);
        free_string(tail->string);
        head = head->next;
        free(tail);
        free_markdown();
    }
}
