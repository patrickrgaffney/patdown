/*****
 * markdown.c -- markdown queue methods and data structures
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-30
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

/** alloc_markdown() -- allocate space for a new Markdown node **********/
static Markdown *alloc_markdown(void)
{
    Markdown *node = NULL;
    node = malloc(sizeof(Markdown));
    if (!node) throw_fatal_memory_error();
    return node;
}


/** init_markdown(s, start, stop, type) -- init new markdown_t node *****/
Markdown *init_markdown(String *s, const size_t start, 
                        const size_t stop, const mdblock_t type)
{
    Markdown *node = alloc_markdown();
    
    /* Use the indexes obtained from parsing the raw input line to
     * create a substring to be used for inline parsing and output. */
    node->value = create_substring(s, start, stop);
    node->type  = type;
    node->next  = NULL;
    node->data  = NULL;
    return node;
}


/** insert_markdown_queue(head, tail, temp) -- insert temp at queue's tail */
void insert_markdown_queue(Markdown **head, Markdown **tail, Markdown *temp)
{
    if (temp) {
        if (!*head) *head = temp;
        else (*tail)->next = temp;
        *tail = temp;
    }
}


/** print_markdown_queue(node) -- debug-print a markdown_t node *********/
void print_markdown_queue(Markdown *node)
{
    if (node) {
        printf("%d: \'%s\'\n", node->type, node->value->string);
        print_markdown_queue(node->next);
    }
}


/** free_markdown(node) -- free all memory allocated for a Markdown node */
void free_markdown(Markdown *node)
{
    if (node) {
        
        /* TODO: free the node->data pointer by determining the 
         *       correct function to call via node->type. */
        
        free_string(node->value);
        free_markdown(node->next);
        free(node);
    }
}


/************************************************************************
 * @section Specific Markdown Data Information
 ************************************************************************/

/** alloc_code_block_data() - allocate memory for new md_code_block_t node */
CodeBlock *alloc_code_block_data(void)
{
    CodeBlock *node = NULL;
    node = malloc(sizeof(CodeBlock));
    if (!node) throw_fatal_memory_error();
    return node;
}