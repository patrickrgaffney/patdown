/* markdown.c
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 02/25/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This file contains the implementations of functions and 
 * enumerations that create and iterate over a queue data structure
 * created from the block-level elements of a MD file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "markdown.h"
#include "files.h"
#include "parsers.h"
#include "errors.h"


/* Build a queue (FIFO) of lines parsed from inputFile. */
block_node_t *buildQueue(FILE *inputFile)
{
    // List initially empty
    block_node_t *headNode = NULL;
    block_node_t *tailNode = NULL;
    
    mdblock_t lastBlockType       = UNKNOWN; // Last block that was parsed
    mdblock_t lastQueuedBlockType = UNKNOWN; // Last block that was added to queue
    
    // Consume all lines from inputFile :: Break when readLine() returns NULL
    while (1)
    {
        // Read line from file :: return NULL if EOF
        char *line = readLine(inputFile);
        temp_block_node_t *currentBlock;
        
        // EOF reached
        if (line == NULL) { break; }
        else
        {
            currentBlock = parseBlockType(line, lastBlockType);
            
            if (currentBlock->blockType == PARAGRAPH && lastBlockType == PARAGRAPH)
            {
                // Append the two lines together -- they constitute the same PARAGRAPH
                tailNode->blockString = reallocateString(tailNode->blockString, currentBlock->blockString);
            }
            else if (currentBlock->blockType == INDENTED_CODE_BLOCK && lastQueuedBlockType == INDENTED_CODE_BLOCK)
            {
                // Two INDENTED_CODE_BLOCKs in a row -- Append them together into one
                tailNode->blockString = reallocateString(tailNode->blockString, currentBlock->blockString);
            }
            else if (currentBlock->blockType == BLANK_LINE && lastQueuedBlockType == INDENTED_CODE_BLOCK)
            {
                // BLANK_LINE encountered while "inside" INDENTED_CODE_BLOCK -- append a newline
                tailNode->blockString = reallocateString(tailNode->blockString, "");
            }
            else if (currentBlock->blockType == SETEXT_HEADING_1 && lastQueuedBlockType == PARAGRAPH)
            {
                // Change the last line (PARAGRAPH) to a SETEXT_HEADING_1
                tailNode->blockType = SETEXT_HEADING_1;
            }
            else if (currentBlock->blockType == SETEXT_HEADING_2 && lastQueuedBlockType == PARAGRAPH)
            {
                // Change the last line (PARAGRAPH) to a SETEXT_HEADING_2
                tailNode->blockType = SETEXT_HEADING_2;
            }
            else if (currentBlock->blockType != BLANK_LINE)
            {
                insertBlockNode(&headNode, &tailNode, currentBlock);
                lastQueuedBlockType = currentBlock->blockType;
            }
            lastBlockType = currentBlock->blockType;
        }
    }
    
    return headNode;
}

/* Insert a blockNode, `newBlock` into the queue at the tail. */
void insertBlockNode(block_node_t **head, block_node_t **tail, temp_block_node_t *temp)
{
    if (temp->blockString != NULL)
    {
        // Create node to be inserted
        block_node_t *newNode = malloc(sizeof(block_node_t));
        
        if (newNode != NULL)
        {
            // Assign the new attributes to node
            newNode->blockString = temp->blockString;
            newNode->blockType = temp->blockType;
            newNode->nextBlockNode = NULL;
            newNode->nextInlineNode = NULL;
            
            // If queue is empty, insert at head
            if (*head == NULL) { *head = newNode; }

            // Otherwise, insert at the end
            else { (*tail)->nextBlockNode = newNode; }
        
            // Reassign the tail to be this new node
            *tail = newNode;
        }
        else
        {
            atexit(printMemoryError);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        printf("ERROR: newNode has not been allocated.\n");
        exit(EXIT_FAILURE);
    }
}


/* Print the queue, line-by-line, by calling `writeLine(fp, i, ...)`. */
void printQueue(block_node_t *currentNode, FILE *outputFile)
{
    // If queue is empty, exit with message
    if (currentNode == NULL) { printf("\nQueue is empty.\n"); }
    else
    {
        // Loop until currentNode->nextBlockNode is NULL
        while (currentNode != NULL)
        {
            switch (currentNode->blockType)
            {
                case SETEXT_HEADING_1:
                case ATX_HEADING_1: 
                    writeLine(outputFile, 3, "<h1>", currentNode->blockString, "</h1>");
                    break;
                case SETEXT_HEADING_2:
                case ATX_HEADING_2: 
                    writeLine(outputFile, 3, "<h2>", currentNode->blockString, "</h2>");
                    break;
                case ATX_HEADING_3: 
                    writeLine(outputFile, 3, "<h3>", currentNode->blockString, "</h3>");
                    break;
                case ATX_HEADING_4: 
                    writeLine(outputFile, 3, "<h4>", currentNode->blockString, "</h4>");
                    break;
                case ATX_HEADING_5: 
                    writeLine(outputFile, 3, "<h5>", currentNode->blockString, "</h5>");
                    break;
                case ATX_HEADING_6: 
                    writeLine(outputFile, 3, "<h6>", currentNode->blockString, "</h6>");
                    break;
                case PARAGRAPH:
                    writeLine(outputFile, 3, "<p>", currentNode->blockString, "</p>");
                    break;
                case INDENTED_CODE_BLOCK:
                    writeLine(outputFile, 3, "<pre><code>", currentNode->blockString, "\n</code></pre>");
                    break;
                case HORIZONTAL_RULE:
                    writeLine(outputFile, 1, "<hr />");
                    break;
                default:
                    writeLine(outputFile, 1, currentNode->blockString);
            }
            
            // Advance to next node in the stack
            currentNode = currentNode->nextBlockNode;
        }
    }   
}