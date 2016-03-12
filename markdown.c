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


/* Build a linked-list of lines parsed from inputFile. */
block_node_t *buildQueue(FILE *inputFile)
{
    // List initially empty
    block_node_t *headNode = NULL;
    block_node_t *tailNode = NULL;
    
    // Consume all lines from inputFile
    // Break when readLine() returns NULL
    while (1)
    {
        // Read line from file :: return NULL if EOF
        char *line = readLine(inputFile);
        temp_block_node_t *currentBlock;
        
        // EOF reached
        if (line == NULL) { break; }
        else
        {
            currentBlock = parseBlockType(line);
            insertBlockNode(&headNode, &tailNode, currentBlock);
        }
    }
    
    return headNode;
}


/* Parse a line of text and attempt to determine its blockType. 
 * Return a tempBlock structure containing:
 *  1. The assigned blockType.
 *  2. The string containing only the characters to be written to the
 *     outputFile.
 *      - AKA: Stripped of its metacharacters.
 *
 * This is accomplished by scanning the first four characters of each
 * line for unique syntactical tokens to markdown. A function of the 
 * form `isBlockElement()` is called to determine the validity of 
 * each assumption.
 */
temp_block_node_t *parseBlockType(const char *line)
{
    // tempBlock is initially unknown to us
    temp_block_node_t *block = malloc(sizeof(temp_block_node_t));
    block->blockType   = UNKNOWN;
    block->blockString = NULL;
    
    // TODO: Increment a `lastBlock` variable to be EMPTY_LINE
    //       - EMPTY_LINE's are required be some block types
    //       - call readLine() again, get new *line
    //       - parse again with knowledge of previous line.
    if (strlen(line) == 0) { return block; }
    
    // iterate through first 4 characters of line
    for (size_t i = 0; i < 4; i++)
    {
        switch (line[i])
        {
            case ' ': // INDENTED_CODE_BLOCK
                break;
            case '#': // ATX_HEADER 
                *block = isATXHeader(&line[i]);
                break;
        }
        
        // TODO: figure out something clever to do here
        if (block->blockType != UNKNOWN) { break; }
    }
    
    // TODO: THIS IS A DIRTY HACK TO BE REMOVED AT A LATER/SMARTER TIME
    if (block->blockType == PARAGRAPH || block->blockType == UNKNOWN)
    {
        block->blockString = allocateString(line, 0, strlen(line));
    }
    
    return block;
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
                case ATX_HEADING_1: 
                    writeLine(outputFile, 3, "<h1>", currentNode->blockString, "</h1>");
                    break;
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
                default:
                    writeLine(outputFile, 1, currentNode->blockString);
            }
            
            // Advance to next node in the stack
            currentNode = currentNode->nextBlockNode;
        }
    }   
}