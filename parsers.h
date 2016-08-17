/* 
 * parsers.h -- markdown parsing methods
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 07/11/2016.
 * 
 *********ultrapatbeams*/

#ifndef PARSERS_H
#define PARSERS_H

#include "markdown.h"

/***** block_parser(fp) -- determine the parsing function to call **/
/******* NOTE: this is the external API for the parser *************/
markdown_t *block_parser(FILE *fp);

#endif