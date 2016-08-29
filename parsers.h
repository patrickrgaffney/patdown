/*****
 * parsers.h -- markdown parsing methods
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-08-29
 * 
 *****************************************************************************/

#ifndef PARSERS_H
#define PARSERS_H

#include "markdown.h"

/*****
 * Parse an input file into markdown.
 *
 * This is the external API for the parser.
 *
 * @param   fp  An input file pointer opened for reading.
 *
 * @warning This function will return `NULL` if the file pointer has not been
 *          opened. This should be interpreted as an error.
 *
 * @return  A queue of parsed, markdown nodes.
 *****************************************************************************/
markdown_t *markdown(FILE *fp);

#endif