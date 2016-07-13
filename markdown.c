/* 
 * markdown.c -- markdown structures and methods.
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 07/12/2016.
 * 
 *********ultrapatbeams*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "markdown.h"
#include "strings.h"
#include "errors.h"
#include "parsers.h"
#include "files.h"


/******************************************************************
 * markdown() -- convert a markdown file to an output type
 *
 * FILE *inputFile -- input markdown file
 *
 * @return -- markdown_t stack or NULL (if inputFile was empty)
 ******************************************************************/
markdown_t *markdown(FILE *inputFile)
{
    markdown_t *head = NULL, *tail = NULL, *temp = NULL;
    string_t *rawBlock;
    if (!inputFile) return NULL;
    
    while (true) {
        rawBlock = read_line(inputFile);
        if (rawBlock->len == 0 && feof(inputFile)) break;
        temp = block_parser(rawBlock);
        
        // Either update the queue or insert this new node.
        if (update_queue(&tail, temp)) free_markdown(temp);
        else insert_markdown_queue(&head, &tail, temp);
        
        free_stringt(rawBlock);
    }
    free_stringt(rawBlock);
    return head;
}


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
        free_stringt(node->value);
        free_markdown(node->next);
        free(node);
    }
}


/******************************************************************
 * update_queue() -- update the last node based on new information
 *
 * markdown_t **tail -- node at tail of queue
 * markdown_t  *temp -- (new) node that was most recently parsed
 *
 * @return -- true if queue was updated, false if not
 ******************************************************************/
bool update_queue(markdown_t **tail, markdown_t *temp)
{
    if (!*tail) return false;
    
    switch (temp->type) {
        case SETEXT_HEADER_1:
        case SETEXT_HEADER_2:
            (*tail)->type = temp->type;
            return true;
        case PARAGRAPH:
            if ((*tail)->type == PARAGRAPH) goto combine;
            break;
        case FENCED_CODE_BLOCK:
        case INDENTED_CODE_BLOCK:
            if ((*tail)->type == INDENTED_CODE_BLOCK) goto combine_newline;
            else if ((*tail)->type == FENCED_CODE_BLOCK) goto combine_newline;
            break;
        default: break;
    }
    return false;
    
    combine: // Append *temp to **tail -- separated by a space.
        (*tail)->value = combine_strings((*tail)->value, temp->value, false);
        free_markdown(temp);
        return true;
    
    combine_newline: // Append *temp to **tail -- separated by a newline.
        (*tail)->value = combine_strings((*tail)->value, temp->value, true);
        return true;
}