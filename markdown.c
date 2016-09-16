/*****
 * markdown.c -- markdown queue methods and data structures
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-15
 * 
 ************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "block_types.h"
#include "errors.h"
#include "markdown.h"
#include "parsers.h"
#include "strings.h"


/************************************************************************
 * @section Markdown Queue Methods
 ************************************************************************/

/**
 * alloc_markdown() -- allocate space for a new markdown_t node
 **
 * @throws -- throw_memory_error()
 * @return -- a pointer to a markdown_t node
 ************************************************************************/
static markdown_t *alloc_markdown(void)
{
    markdown_t *node = NULL;
    node = malloc(sizeof(markdown_t));
    if (!node) throw_memory_error();
    return node;
}


/**
 * init_markdown(s, start, stop, type) -- alloc/init new markdown_t node
 **
 *  Uses the indexes obtained from parsing the raw input line to
 *  create a substring of the raw block that will be used for output.
 **
 * @return -- a pointer to a allocated and initialized markdown_t node
 ************************************************************************/
markdown_t *init_markdown(string_t *s, const size_t start, 
                          const size_t stop, const mdblock_t type)
{
    markdown_t *node = alloc_markdown();
    node->value = create_substring(s, start, stop);
    node->type  = type;
    node->next  = NULL;
    node->data  = NULL;
    return node;
}


/**
 * insert_markdown_queue(head, tail, temp) -- insert temp at queue's tail
 ************************************************************************/
void insert_markdown_queue(markdown_t **head, markdown_t **tail, 
                           markdown_t *temp)
{
    if (temp) {
        if (!*head) *head = temp;
        else (*tail)->next = temp;
        *tail = temp;
    }
}


/**
 * print_markdown_queue(node) -- debug-print a markdown_t node
 **
 *  This function is used for debugging purposes only. It initally 
 *  receives the *head* of the queue as the parameter, then makes 
 *  recursive calls to print every node in the queue.
 *****************************************************************************/
void print_markdown_queue(markdown_t *node)
{
    if (node) {
        printf("%d: \'%s\'\n", node->type, node->value->string);
        print_markdown_queue(node->next);
    }
}


/**
 * free_markdown(node) -- free the memory allocated for a markdown_t node
 **
 *  Free *all* the memory associated with the node in 
 *  questions -- that includes it's `string_t` node.
 ************************************************************************/
void free_markdown(markdown_t *node)
{
    if (node) {
        
        /* TODO: free the node->data pointer by determining the 
         *       correct function to call via node->type. */
        
        free_stringt(node->value);
        free_markdown(node->next);
        free(node);
    }
}


/************************************************************************
 * @section Specific Markdown Data Information
 **
 *  TODO: Move alloc_link_reference_data() to a new links.(c|h) file.
 ************************************************************************/

/**
 * alloc_code_block_data() - allocate space for new md_code_block_t node
 **
 * @throws -- throw_memory_error()
 * @return -- a pointer to a md_code_block_t node
 ************************************************************************/
md_code_block_t *alloc_code_block_data(void)
{
    md_code_block_t *node = NULL;
    node = malloc(sizeof(md_code_block_t));
    if (!node) throw_memory_error();
    return node;
}


/**
 * alloc_link_reference_data() - allocate space for new link_ref_t node
 **
 * @throws -- throw_memory_error()
 * @return -- a pointer to a link_ref_t node
 ************************************************************************/
link_ref_t *alloc_link_reference_data(void)
{
    link_ref_t *link = NULL;
    link = malloc(sizeof(link_ref_t));
    if (!link) throw_memory_error();
    return link;
}
