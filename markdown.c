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
 * enumerations that operate on text, transforming it into markdown.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "markdown.h"
#include "files.h"
#include "parsers.h"


/* Parse a line of text, returning the MDBlockType of that line. */
blockNode *buildList(FILE *inputFile)
{
    // List initially empty
    blockNode *headNode = NULL;
    blockNode *tailNode = NULL;
    
    // Consume all lines from inputFile
    while (1)
    {
        // Read line from file :: return NULL if EOF
        char *line = readLine(inputFile);
        blockNode currentBlock;
        
        // EOF reached
        if (line == NULL) 
        { 
            break;
        }
        else
        {
            currentBlock = parseBlockType(line);
            // insertBlockNode(&startNode, currentBlock);
        }
        
        if (currentBlock.blockString != NULL)
        {
            printf("%s\n", currentBlock.blockString);
        }
    }
    return headNode;
}


/* Parse a line of text and attempt to determine its MDBlockType. 
 * Return a mdBlock structure containing:
 *  1. The assigned MDBlockType.
 *  2. The string containing only the characters to be written to the
 *     outputFile.
 *      - AKA: Stripped of its metacharacters.
 *
 * This is accomplished by scanning the first four characters of each
 * line for unique syntactical tokens to markdown. A function of the 
 * form `isBlockElement()` is called to determine the validity of 
 * each assumption.
 */
blockNode parseBlockType(const char *line)
{
    blockNode tempBlock;
    tempBlock.blockType = UNKNOWN;
    tempBlock.blockString = NULL;
    
    if (strlen(line) == 0)
    {
        tempBlock.blockType = UNKNOWN;
        return tempBlock;
    }
    
    for (size_t i = 0; i < 4; i++)
    {
        switch (line[i])
        {
            case ' ':
                break;
            
            case '#':
                tempBlock = isATXHeader(&line[i]);
                break;
        }
        
        if (tempBlock.blockType != UNKNOWN) 
        {
            break;
        }
    }
    
    if (tempBlock.blockType == PARAGRAPH || tempBlock.blockType == UNKNOWN)
    {
        tempBlock.blockString = allocateString(line, 0, strlen(line));
    }
    
    return tempBlock;
}


/* Insert a blockNode, `newBlock` into the queue at the tail. */
// void insertBlockNode(blockNode **head, blockNode **tail, blockNode newNode)
// {
//     if (newNode.blockString != NULL && newNode.blockType != UNKNOWN)
//     {
//         newNode.nextBlockNode = NULL;
//         newNode.nextInlineNode = NULL;
//
//         // If queue is empty, insert at head
//         if (head == NULL)
//         {
//             **head = newNode;
//         }
//         else
//         {
//             *(*tail)->nextBlockNode = newNode;
//         }
//     }
//     else
//     {
//         printf("ERROR: newNode has not been allocated.\n");
//         exit(EXIT_FAILURE);
//     }
// }