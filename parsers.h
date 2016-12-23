/**
 * parsers.h -- markdown parsing methods
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2016-10-24
 *  project:    patdown
 * 
 *****************************************************************************/

#pragma once

#include "strings.h"

/** Call upon the parsers and generate the Markdown queue. */
bool markdown(String *rawBytes);
