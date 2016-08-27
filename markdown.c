/* 
 * markdown.c -- markdown structures and methods.
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 07/20/2016.
 * 
 *********ultrapatbeams*/

#include <stdio.h>      // for input files
#include <stdlib.h>     // for allocation & freeing
#include <stdbool.h>    // for true & false

#include "markdown.h"
#include "strings.h"
#include "errors.h"
#include "parsers.h"
#include "files.h"


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
markdown_t *init_markdown(string_t *s, const size_t start, const size_t stop, const mdblock_t type)
{
    markdown_t *line = alloc_markdown();
    line->value = create_substring(s, start, stop);
    line->type  = type;
    line->next  = NULL;
    line->data  = NULL;
    return line;
}


/******************************************************************
 * alloc_markdown() -- allocate space for a new markdown_t node
 *
 * @throws -- throw_memory_error() if node cannot be allocated
 * @return -- an allocated markdown_t node
 ******************************************************************/
markdown_t *alloc_markdown(void)
{
    markdown_t *node = NULL;
    node = malloc(sizeof(markdown_t));
    if (!node) throw_memory_error();
    return node;
}


/******************************************************************
 * insert_markdown_queue() -- place a new node at the queue's tail
 *
 * markdown_t **head -- current head of the queue
 * markdown_t **tail -- current tail of the queue
 * markdown_t  *temp -- node to be inserted
 *
 * @noreturn  -- control returned to the caller
 ******************************************************************/
void insert_markdown_queue(markdown_t **head, markdown_t **tail, markdown_t *temp)
{
    if (temp) {
        if (!*head) *head = temp;
        else (*tail)->next = temp;
        *tail = temp;
    }
}


/******************************************************************
 * print_markdown_queue() -- print the queue to stdout
 *
 * markdown_t *node -- node to be printed
 *
 * @noreturn -- control returned to the caller
 ******************************************************************/
void print_markdown_queue(markdown_t *node)
{
    if (node) {
        printf("%d: \'%s\'\n", node->type, node->value->string);
        print_markdown_queue(node->next);
    }
}


/******************************************************************
 * alloc_markdown() -- free markdown_t node, if it exists
 *
 * markdown_t *node -- node to be freed
 *
 * @recursive -- calls node->next for every node
 * @noreturn  -- control returned to the caller
 ******************************************************************/
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