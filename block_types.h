/***** 
 * block_types.h -- extensions to basic markdown_t node
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-07-28
 * @modified    2016-09-30
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

#endif