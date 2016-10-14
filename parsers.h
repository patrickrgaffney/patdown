/*****
 * parsers.h -- markdown parsing methods
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2016-10-13
 *  project:    patdown
 * 
 *****************************************************************************/

#pragma once

#include "markdown.h"
#include "output.h"
#include "strings.h"

/*****************************************************************************
 * @section External Parsing API
 *****************************************************************************/
bool markdown(String *rawBytes);
