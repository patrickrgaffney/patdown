/*****
 * html.h -- html output writer
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-10-04
 * @modified    2016-10-04
 * 
 ************************************************************************/

#include <stdio.h>

#include "markdown.h"

void output_html(FILE *fp, Markdown *node);