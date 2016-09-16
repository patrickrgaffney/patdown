/***** 
 * block_types.h -- extensions to basic markdown_t node
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-07-28
 * @modified    2016-09-15
 * 
 *	TODO: Move all link references and function definitions to their own
 *		  file -- links.(c|h)
 *
 *	TODO: Move md_code_block_t to markdown.h file -- its functions are
 *		  already there.
 ************************************************************************/

#ifndef BLOCK_TYPES_H
#define BLOCK_TYPES_H

#include <stddef.h>
#include <stdbool.h>

/**
 * md_code_block_t -- markdown_t extension for fenced code blocks
 ************************************************************************/
typedef struct md_code_block_t
{
    char lang[20];		/* User-provided language for the code block. 	*/
} md_code_block_t;


/**
 * link_ref_t -- node to hold link data information
 **
 *	These nodes will be used to create a BST of link references.
 ************************************************************************/
typedef struct link_ref_t
{
    char link[1000];	/* Link label -- unique identifier for links. 	*/
    char dest[1000];	/* Link destination -- the URL to link to.		*/
    char title[1000];	/* Link title -- optional, <a title> attribute.	*/
} link_ref_t;

#endif