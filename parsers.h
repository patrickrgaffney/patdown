/*****
 * parsers.h -- markdown parsing methods
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-09-15
 * 
 *****************************************************************************/

#ifndef PARSERS_H
#define PARSERS_H

#include "markdown.h"

/*****************************************************************************
 * @section External Parsing API
 *****************************************************************************/
markdown_t *markdown(FILE *fp);

#endif