/*****
 * markdown.c -- markdown queue methods
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-08-29
 * 
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "markdown.h"
#include "strings.h"
#include "errors.h"
#include "parsers.h"
#include "files.h"
#include "block_types.h"


/******************************************************************************
 * @section Markdown Queue Methods
 *****************************************************************************/

/*****
 * Allocate space for a new `markdown_t` node.
 *
 * @throws  throw_memory_error()
 * @return  An allocated `markdown_t` node.
 *****************************************************************************/
static markdown_t *alloc_markdown(void)
{
    markdown_t *node = NULL;
    node = malloc(sizeof(markdown_t));
    if (!node) throw_memory_error();
    return node;
}


/*****
 * Create a new `markdown_t` node.
 *
 * This function uses the indexes obtained from parsing the raw input line to
 * create the substring of the markdown block that will be used for output.
 *
 * @param   s       Original `string_t` node read from input file.
 * @param   start   Index where the substring should start.
 * @param   stop    Index where the substring should stop.
 * @param   type    Type (`mdblock_t`) on the new `markdown_t` node.
 *
 * @return  An initialized `markdown_t` node.
 *****************************************************************************/
markdown_t *init_markdown(string_t *s, const size_t start, 
                          const size_t stop, const mdblock_t type)
{
    markdown_t *line = alloc_markdown();
    line->value = create_substring(s, start, stop);
    line->type  = type;
    line->next  = NULL;
    line->data  = NULL;
    return line;
}


/*****
 * Insert a new `markdown_t` node at the tail of the queue.
 *
 * @param   head    Current head of the markdown queue.
 * @param   tail    Current tail of the markdown queue.
 * @param   temp    Node to be inserted to the queue.
 *****************************************************************************/
void insert_markdown_queue(markdown_t **head, markdown_t **tail, markdown_t *temp)
{
    if (temp) {
        if (!*head) *head = temp;
        else (*tail)->next = temp;
        *tail = temp;
    }
}


/*****
 * Print the markdown queue to standard output.
 *
 * This function is used for debugging purposes only. It initally receives the
 * *head* of the queue as the parameter, then makes recursive calls to print
 * every node in the queue.
 *
 * @param   node    The current node to be printed.
 *****************************************************************************/
void print_markdown_queue(markdown_t *node)
{
    if (node) {
        printf("%d: \'%s\'\n", node->type, node->value->string);
        print_markdown_queue(node->next);
    }
}


/*****
 * Free the memory allocated for a `markdown_t` node.
 *
 * @warning This function will free *all* the memory associated with the node
 *          in questions -- that includes it's `string_t` node.
 *
 * @param   node    The node to be free'd.
 *****************************************************************************/
void free_markdown(markdown_t *node)
{
    if (node) {
        // TODO: free the node->data pointer by determining the 
        // correct function to call via node->type
        
        free_stringt(node->value);
        free_markdown(node->next);
        free(node);
    }
}


/******************************************************************************
 * @section Specific Markdown Data Information
 *****************************************************************************/

/*****
 * Allocate space for a new `md_code_block_t` node.
 *
 * @throws  throw_memory_error()
 * @return  An allocated `md_code_block_t` node.
 *****************************************************************************/
md_code_block_t *alloc_code_block_data(void)
{
    md_code_block_t *node = NULL;
    node = malloc(sizeof(md_code_block_t));
    if (!node) throw_memory_error();
    return node;
}

link_ref_t *alloc_link_reference_data(void)
{
    link_ref_t *link = NULL;
    link = malloc(sizeof(link_ref_t));
    if (!link) throw_memory_error();
    return link;
}