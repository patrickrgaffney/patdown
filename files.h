/**
 * files.h -- opening, closing, and reading input from files
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-17
 *  modified:   2016-12-22
 *  project:    patdown
 * 
 ************************************************************************/

#pragma once

#include <stdio.h>

#include "strings.h"

/************************************************************************
 * # Opening & Closing Files
 ************************************************************************/

/** Open a file stream for some given file access mode. */
FILE *open_file(const char *fileName, const char *mode);

/************************************************************************
 * # Reading Input From Files
 ************************************************************************/

/** Read all bytes from a supplied input file stream. */
String *read_all_input_bytes(FILE *ifp);
