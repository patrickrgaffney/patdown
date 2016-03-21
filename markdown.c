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
    MarkdownBlock *headNode = NULL;
    MarkdownBlock *tailNode = NULL;
    
    mdblock_t lastBlockType       = UNKNOWN; // Last block that was parsed
    mdblock_t lastQueuedBlockType = UNKNOWN; // Last block that was added to queue
    
    char *line = NULL;
    
    while (1)
    {
        line = readLine(inputFile);
        TempMarkdownBlock *currentBlock;
        
        if (line == NULL) break;
        else
        {
            currentBlock = parse_block_type(line, lastBlockType);
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
                case INSERT_NODE:
                    insert_block_node(&headNode, &tailNode, currentBlock);
                    lastQueuedBlockType = currentBlock->blockType;
                    break;
            }
            lastBlockType = currentBlock->blockType;
        }
        free(currentBlock);
    }
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
    if (temp->blockString != NULL)
    {
        MarkdownBlock *newNode = alloc_block(temp->blockString, temp->blockType);
        
        if (newNode != NULL)
        {   
            // If queue is empty, insert at head; otherwise insert at tail.
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
    if (currentNode == NULL) printf("\nNo input was provided.\n");
    else
    {
        while (currentNode != NULL)
        {
            switch (currentNode->blockType)
            {
                case SETEXT_HEADING_1:
                case ATX_HEADING_1: 
                    writeLine(outputFile, 3, "<h1>", currentNode->blockString, "</h1>"); break;
                case SETEXT_HEADING_2:
                case ATX_HEADING_2: 
                    writeLine(outputFile, 3, "<h2>", currentNode->blockString, "</h2>"); break;
                case ATX_HEADING_3: 
                    writeLine(outputFile, 3, "<h3>", currentNode->blockString, "</h3>"); break;
                case ATX_HEADING_4: 
                    writeLine(outputFile, 3, "<h4>", currentNode->blockString, "</h4>"); break;
                case ATX_HEADING_5: 
                    writeLine(outputFile, 3, "<h5>", currentNode->blockString, "</h5>"); break;
                case ATX_HEADING_6: 
                    writeLine(outputFile, 3, "<h6>", currentNode->blockString, "</h6>"); break;
                case PARAGRAPH:
                    writeLine(outputFile, 3, "<p>", currentNode->blockString, "</p>"); break;
                case FENCED_CODE_BLOCK:
                case INDENTED_CODE_BLOCK:
                    writeLine(outputFile, 3, "<pre><code>", currentNode->blockString, "</code></pre>"); break;
                case HORIZONTAL_RULE:
                    writeLine(outputFile, 1, "<hr />"); break;
                case HTML_BLOCK:
                    writeLine(outputFile, 1, currentNode->blockString); break;
                case HTML_COMMENT:
                    writeLine(outputFile, 1, currentNode->blockString); break;
                case FENCED_CODE_BLOCK_START:
                    writeLine(outputFile, 2, "START: ", currentNode->blockString); break;
                case FENCED_CODE_BLOCK_END:
                    writeLine(outputFile, 2, "END: ", currentNode->blockString); break;
                default:
                    writeLine(outputFile, 1, currentNode->blockString);
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
    block->blockString    = s;
    block->blockType      = type;
    block->nextBlockNode  = NULL;
    block->nextInlineNode = NULL;
    return block;
}


/* alloc_temp_block(char *, mdblock_t, insert_t)
 * =======================================================================
 * Allocate space for a TempMarkdownBlock structure, initialize it with 
 * the string, mdblock_t, and insert_t that were passed to the function.
 *
 * Return a pointer to the newly initialized TempMarkdownBlock structure.
 * NOTE: Function will exit program if the malloc returns NULL.
 * ======================================================================= */
TempMarkdownBlock *alloc_temp_block(char *s, mdblock_t type, insert_t insert)
{
    TempMarkdownBlock *block = malloc(sizeof(TempMarkdownBlock));
    if (block)
    {
        block->blockString       = s;
        block->blockType         = type;
        block->insertType        = insert;
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