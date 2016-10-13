/*****
 * output.h -- methods for calling output generators
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-10-04
 * @modified    2016-10-13
 * 
 ************************************************************************/

#include <stdio.h>

#include "markdown.h"


/*****
 * Valid ouput type constants.
 *
 *  These constants are used to differentiate between the valid output 
 *  types supported by patdown.
 * 
 *  TODO: Add support for more types: RTF, PDF (...maybe...)
 *****/
typedef enum 
{
    OUT_HTML5,      /* Default: HTML5 element syntax. */ 
    OUT_PARSED      /* Internal parsing information (for debugging). */
} output_t;


// void output_html(FILE *fp, Markdown *node);
