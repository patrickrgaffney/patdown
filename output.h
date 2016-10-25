/**
 * output.h -- methods and types for generating output
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-10-04
 *  modified:   2016-10-24
 *  project:    patdown
 * 
 ************************************************************************/

#pragma once


/** Valid ouput type constants. **/
typedef enum 
{
    OUT_HTML5,      /* Default: HTML5 element syntax. */ 
    OUT_PARSED      /* Internal parsing information (for debugging). */
} output_t;
