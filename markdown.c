/*****
 * markdown.c -- markdown queue methods
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-10-05
 * 
 ************************************************************************/

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

/*****
 * Allocate memory for a new Markdown block.
 *
 * ERRORS
 *  fatal_memory_error  Memory could not be allocated.
 *
 * RETURNS
 *  A pointer to the new Markdown node.
 *****/
static Markdown *__alloc_markdown_node(void)
{
    Markdown *node = NULL;
    node = malloc(sizeof(Markdown));
    if (!node) throw_fatal_memory_error();
    return node;
}

/*****
 * Allocate a Markdown block with a String and mdblock_t.
 *
 * ARGUMENTS
 *  s       The String node for this particular Markdown block.
 *  type    The type of this Markdown block.
 *
 * RETURNS
 *  A pointer to the new Markdown block.
 *****/
Markdown *init_markdown(String *s, const mdblock_t type)
{
    Markdown *node = __alloc_markdown_node();
    node->string   = s;
    node->type     = type;
    node->next     = NULL;
    node->addtinfo = NULL;
    return node;
}


/************************************************************************
 * Markdown Queue Manipulation
 ************************************************************************/

/*****
 * Insert a Markdown block into the queue at the tail.
 *
 * ARGUMENTS
 *  head    The first node in the queue.
 *  tail    The last node in the queue.
 *  node    The node to be inserted at the tail.
 *****/
void insert_queue_tail(Markdown **head, Markdown **tail, Markdown *node)
{
    if (node) {
        if (!*head) *head = node;
        else (*tail)->next = node;
        *tail = node;
    }
}

/*****
 * Debug-print the entire Markdown queue.
 *
 *  This function is used for debugging purposes only.
 *
 * ARGUMENTS
 *  node    The current node to print.
 *****/
void debug_print_queue(Markdown *node)
{
    if (node) {
        printf("%d: \'%s\'\n", node->type, node->string->data);
        debug_print_queue(node->next);
    }
}

/*****
 * Free all the Markdown nodes in the queue.
 *
 * ARGUMENTS
 *  node    The current node to free.
 *****/
void free_markdown(Markdown *node)
{
    if (node) {
        if (node->addtinfo) free(node->addtinfo);
        free_string(node->string);
        free_markdown(node->next);
        free(node);
    }
}


/************************************************************************
 * Markdown Block Extensions
 ************************************************************************/

/*****
 * Allocate memory for a new CodeBlock info node.
 *
 * ERRORS
 *  fatal_memory_error  Memory could not be allocated.
 *
 * RETURNS
 *  A pointer to the new CodeBlock node.
 *****/
CodeBlock *alloc_code_block_info(void)
{
    CodeBlock *code = NULL;
    code = malloc(sizeof(CodeBlock));
    if (!code) throw_fatal_memory_error();
    return code;
}
