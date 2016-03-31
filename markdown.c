/* markdown.c
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 02/25/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file contains the definitions of functions, structures, and 
 * enumerations that create, maintain, and print a queue of MarkdownBlock
 * nodes that serve as the main data structure through which the program
 * flows.
 * ======================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "markdown.h"
#include "files.h"
#include "parsers.h"
#include "errors.h"
#include "utilities.h"


/* buildQueue(FILE *)
 * =======================================================================
 * Build a queue of parsed lines, each read sequentially from the input
 * file. This function calles parse_block_type() and the individual 
 * parsers are called from there.
 *
 * Returns a pointer to the head of the queue.
 * ======================================================================= */
MarkdownBlock *build_queue(FILE *inputFile)
{
    MarkdownBlock *headNode = NULL; // Head of the queue of MarkdownBlock's.
    MarkdownBlock *tailNode = NULL; // Tail of the queue of MarkdownBlock's.
    LinkRef *linkTree       = NULL; // Root of the tree of LinkRef's.
    char *line              = NULL; // A line read from the input File.
    
    while (1)
    {
        line = read_line(inputFile);
        TempMarkdownBlock *currentBlock;
        
        if (!line) break;
        else
        {
            currentBlock = parse_block_type(line);
            free(line);
            
            switch (currentBlock->insertType)
            {
                case APPEND_STRING: 
                    tailNode->blockString = combine_strings_newline(tailNode->blockString, currentBlock->blockString); break;
                case APPEND_NEWLINE:
                    tailNode->blockString = combine_strings_newline(tailNode->blockString, ""); break;
                case UPDATE_TYPE:
                    tailNode->blockType = currentBlock->blockType; break;
                case PLACEHOLDER: break;
                case INSERT_LINK:
                    insert_link_ref(&linkTree, currentBlock->linkReference); break;
                case INSERT_NODE:
                    insert_block_node(&headNode, &tailNode, currentBlock); break;
            }
        }
        free(currentBlock);
    }
    print_tree(linkTree);
    free_tree(linkTree);
    return headNode;
}

/* insert_block_node(MarkdownBlock, MarkdownBlock, TempMarkdownBlock)
 * =======================================================================
 * Create a new MarkdownBlock node, using the information obtained from 
 * parsing in the TempMarkdownBlock. Insert the new node at the tail of
 * the queue; unless **head is NULL, then the **tail is the **head.
 * ======================================================================= */
void insert_block_node(MarkdownBlock **head, MarkdownBlock **tail, TempMarkdownBlock *temp)
{
    if (temp->blockString)
    {
        MarkdownBlock *newNode = alloc_block(temp->blockString, temp->blockType);
        
        if (newNode)
        {
            if (*head == NULL) *head = newNode;
            else (*tail)->nextBlockNode = newNode;

            *tail = newNode;
        }
        else { atexit(print_memory_error); exit(EXIT_FAILURE); }
    }
}


/* print_queue(MarkdownBlock, FILE *)
 * =======================================================================
 * Prints all the blockString and all the inlineString's at currentNode to
 * the output file. Then makes a recursive call to accomplish the same
 * task at the node pointed to by nextBlockNode. Printing is accomplished
 * by making calls to the write_line(FILE *, i, ...) function.
 * ======================================================================= */
void print_queue(MarkdownBlock *currentNode, FILE *outputFile)
{
    if (!currentNode) printf("\nNo input was provided.\n");
    else
    {
        while (currentNode)
        {
            switch (currentNode->blockType)
            {
                case SETEXT_HEADING_1:
                case ATX_HEADING_1: 
                    write_line(outputFile, true, 3, "<h1>", currentNode->blockString, "</h1>"); break;
                case SETEXT_HEADING_2:
                case ATX_HEADING_2: 
                    write_line(outputFile, true, 3, "<h2>", currentNode->blockString, "</h2>"); break;
                case ATX_HEADING_3: 
                    write_line(outputFile, true, 3, "<h3>", currentNode->blockString, "</h3>"); break;
                case ATX_HEADING_4: 
                    write_line(outputFile, true, 3, "<h4>", currentNode->blockString, "</h4>"); break;
                case ATX_HEADING_5: 
                    write_line(outputFile, true, 3, "<h5>", currentNode->blockString, "</h5>"); break;
                case ATX_HEADING_6: 
                    write_line(outputFile, true, 3, "<h6>", currentNode->blockString, "</h6>"); break;
                case PARAGRAPH:
                    write_line(outputFile, true, 3, "<p>", currentNode->blockString, "</p>"); break;
                case INDENTED_CODE_BLOCK:
                    write_line(outputFile, true, 3, "<pre><code>", currentNode->blockString, "</code></pre>"); break;
                case HORIZONTAL_RULE:
                    write_line(outputFile, true, 1, "<hr />"); break;
                case HTML_BLOCK:
                    write_line(outputFile, true, 1, currentNode->blockString); break;
                case FENCED_CODE_BLOCK_START:
                    write_line(outputFile, false, 3, "<pre><code class=\"language-", currentNode->blockString, "\">"); break;
                case FENCED_CODE_BLOCK:
                    write_line(outputFile, true, 1, currentNode->blockString); break;
                case FENCED_CODE_BLOCK_STOP:
                    write_line(outputFile, true, 1, "</code></pre>"); break;
                default:
                    write_line(outputFile, true, 1, currentNode->blockString);
            }
            currentNode = currentNode->nextBlockNode;
        }
    }   
}


/* alloc_block(char *, mdblock_t)
 * =======================================================================
 * Allocate space for a MarkdownBlock structure, initialize it with the 
 * string and mdblock_t that were passed to the function.
 *
 * Return a pointer to the newly initialized MarkdownBlock structure.
 * ======================================================================= */
MarkdownBlock *alloc_block(char *s, mdblock_t type)
{
    MarkdownBlock *block  = malloc(sizeof(MarkdownBlock));
    if (block)
    {
        block->blockString    = s;
        block->blockType      = type;
        block->nextBlockNode  = NULL;
        block->nextInlineNode = NULL;
        return block;
    }
    else
    {
        atexit(print_memory_error);
        exit(EXIT_FAILURE);
    }
}


/* alloc_temp_block(char *, mdblock_t, insert_t)
 * =======================================================================
 * Allocate space for a TempMarkdownBlock structure, initialize it with 
 * the string, mdblock_t, and insert_t that were passed to the function.
 *
 * Return a pointer to the newly initialized TempMarkdownBlock structure.
 * NOTE: Function will exit program if the malloc returns NULL.
 * ======================================================================= */
TempMarkdownBlock *alloc_temp_block(char *s, mdblock_t type, insert_t insert, LinkRef *alink)
{
    TempMarkdownBlock *block = malloc(sizeof(TempMarkdownBlock));
    if (block)
    {
        block->blockString   = s;
        block->blockType     = type;
        block->insertType    = insert;
        block->linkReference = alink;
        return block;
    }
    else
    {
        atexit(print_memory_error);
        exit(EXIT_FAILURE);
    }
}


/* free_block(MarkdownBlock)
 * =======================================================================
 * Free the space at the MarkdownBlock (if it is not NULL) in the 
 * following order:
 *      1. Free blockString (if not NULL).
 *      2. Free space at nextInlineNode by calling freeInlineQueue().
 *      3. Free space at nextBlockNode by making a recursive call.
 *      4. Free space at currentNode.
 * ======================================================================= */
void free_block(MarkdownBlock *currentNode)
{
    if (currentNode != NULL)
    {
        if (currentNode->blockString != NULL) free(currentNode->blockString);
        free_inline_queue(currentNode->nextInlineNode);
        free_block(currentNode->nextBlockNode);
        free(currentNode);
    }
}


/* free_inline_queue(MarkdownInline)
 * =======================================================================
 * Free the space at the MarkdownInline structure (if it is not NULL) in 
 * the following order:
 *      1. Free inlineString (if not NULL).
 *      2. Free space at nextInlineNode by making a recursive call.
 *      3. Free space at currentNode.
 * ======================================================================= */
void free_inline_queue(MarkdownInline *currentNode)
{
    if (currentNode != NULL)
    {
        if (currentNode->inlineString != NULL) free(currentNode->inlineString);
        free_inline_queue(currentNode->nextInlineNode);
        free(currentNode);
    }
}


/* insert_link_ref(LinkRef, char *, char *, char *)
 * =======================================================================
 * Create a new LinkRef node containing the three strings passed as 
 * arguments. Insert the node into the tree according to the value of the 
 * link reference.
 * ======================================================================= */
void insert_link_ref(LinkRef **tree, LinkRef *node)
{
    if (node)
    {
        if (!*tree) *tree = node;
        else
        {
            int rc = strcmp(node->label, (*tree)->label);
            
            if (rc == 0) printf("duplicate value.\n");
            else if (rc < 0) insert_link_ref(&((*tree)->leftNode), node);
            else if (rc > 0) insert_link_ref(&((*tree)->rightNode), node);
        }
    }
    else printf("LinkRef node was never initialized.\n");
}


/* alloc_link_ref(char *, char *, char *)
 * =======================================================================
 * Allocate space for a new LinkRef and initialize it with the three
 * strings passed as arguments.
 * 
 * Return a pointer to the newly initialized LinkRef structure.
 * NOTE: Function will exit program if the malloc returns NULL.
 * ======================================================================= */
LinkRef *alloc_link_ref(char *label, char *url, char *title)
{
    LinkRef *alink = malloc(sizeof(LinkRef));
    if (alink)
    {
        alink->label     = label;
        alink->url       = url;
        alink->title     = title;
        alink->leftNode  = NULL;
        alink->rightNode = NULL;
        return alink;
    }
    else
    {
        atexit(print_memory_error);
        exit(EXIT_FAILURE);
    }
}


/* free_tree(LinkRef *)
 * =======================================================================
 * Free the space at the LinkRef structure (if it is not NULL) in the 
 * following order:
 *      1. Free label member (if not NULL).
 *      2. Free url member (if not NULL).
 *      3. Free title member (if not NULL).
 *      4. Free space at leftNode by making a recursive call.
 *      5. Free space at rightNode by making a recursive call.
 *      3. Free space at currentNode.
 * ======================================================================= */
void free_tree(LinkRef *currentNode)
{
    if (currentNode)
    {
        if (currentNode->label) free(currentNode->label);
        if (currentNode->url) free(currentNode->url);
        if (currentNode->title) free(currentNode->title);
        free_tree(currentNode->leftNode);
        free_tree(currentNode->rightNode);
        free(currentNode);
    }
}


/* print_tree(LinkRef *currentNode)
 * =======================================================================
 * Traverse the tree of LinkRef nodes **in order** and print their 
 * contents to stdout.
 * 
 * NOTE: This function is for debugging purposes only. It it not called
 *       during normal exection, but is included in the API for testing.
 * ======================================================================= */
void print_tree(LinkRef *currentNode)
{
    if (currentNode)
    {
        print_tree(currentNode->leftNode);
        printf("\nlink label = \'%s\'\n", currentNode->label);
        printf("link url   = \'%s\'\n", currentNode->url);
        printf("link title = \'%s\'\n\n", currentNode->title);
        print_tree(currentNode->rightNode);
    }
}