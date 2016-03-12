/* markdown.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    02/25/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This file contains the implementations of functions and 
 * enumerations that create and iterate over the a stack data 
 * structure created from the block-level elements of a MD file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "markdown.h"
#include "files.h"
#include "parsers.h"
#include "errors.h"


/* Parse a line of text, returning the MDBlockType of that line. */
blockNode *buildList(FILE *inputFile)
{
    // List initially empty
    blockNode *headNode = NULL;
    blockNode *tailNode = NULL;
    
    // Consume all lines from inputFile
    // Break when readLine() returns NULL
    while (1)
    {
        // Read line from file :: return NULL if EOF
        char *line = readLine(inputFile);
        tempBlock *currentBlock;
        
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
tempBlock *parseBlockType(const char *line)
{
    // tempBlock is initially unknown to us
    tempBlock *block = malloc(sizeof(tempBlock));
    block->blockType = UNKNOWN;
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
            case ' ':
                break;
            
            case '#': *block = isATXHeader(&line[i]);
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
void insertBlockNode(blockNode **head, blockNode **tail, tempBlock *temp)
{
    if (temp->blockString != NULL)
    {
        // Create node to be inserted
        blockNode *newNode = malloc(sizeof(blockNode));
        
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


/* Print the queue */
void printQueue(blockNode *currentNode, FILE *outputFile)
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