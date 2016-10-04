/*****
 * markdown.c -- markdown queue methods and data structures
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-10-01
 * 
 ************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "markdown.h"
#include "strings.h"


/************************************************************************
 * @section Markdown Queue Methods
 ************************************************************************/

/** Allocate space for a new Markdown node. *****************************/
static Markdown *alloc_markdown(void)
{
    Markdown *node = NULL;
    node = malloc(sizeof(Markdown));
    if (!node) throw_fatal_memory_error();
    return node;
}


/** Initialize a new Markdown node. *************************************/
Markdown *init_markdown(String *s, const size_t start, 
                        const size_t stop, const mdblock_t type)
{
    Markdown *node = alloc_markdown();
    node->value = create_substring(s, start, stop);
    node->type  = type;
    node->next  = NULL;
    node->data  = NULL;
    return node;
}


/** Insert temp at Markdown queue's tail. *******************************/
void insert_markdown_queue(Markdown **head, Markdown **tail, Markdown *temp)
{
    if (temp) {
        if (!*head) *head = temp;
        else (*tail)->next = temp;
        *tail = temp;
    }
}


/** Debug-print a Markdown node. ****************************************/
void print_markdown_queue(Markdown *node)
{
    if (node) {
        printf("%d: \'%s\'\n", node->type, node->value->string);
        print_markdown_queue(node->next);
    }
}


/** Free all memory allocated for a Markdown node. **********************/
void free_markdown(Markdown *node)
{
    if (node) {
        if (node->data) free(node->data);
        free_string(node->value);
        free_markdown(node->next);
        free(node);
    }
}


/************************************************************************
 * @section Specific Markdown Data Information
 ************************************************************************/

/** Allocate memory for new CodeBlock node. *****************************/
CodeBlock *alloc_code_block_data(void)
{
    CodeBlock *node = NULL;
    node = malloc(sizeof(CodeBlock));
    if (!node) throw_fatal_memory_error();
    return node;
}