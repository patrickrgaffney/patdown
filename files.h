/* files.h
 *
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    02/15/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This file contains the definitions of functions that open various
 * types of files (INPUT, OUPTUT) for processing.
 */

#ifndef FILES_H
#define FILES_H

#include <stdio.h>

#include "arguments.h"

/* ==================================================================
 * == FUNCTIONS
 * =============================================================== */

/* Open a file for either reading or writing data. If the file cannot
 * be opened, exit with an error dialog. Otherwise, return the file
 * pointer to the calling function.
 */
FILE *openFile(const char fileName[], const ArgType fileType);


/* Read exactly one line (until next newline) from the file pointed
 * to by *fp.
 */
char *readLine(FILE *fp);


/* Wrtie exactly one line (received as input) to the file pointed to
 * by *fp.
 */
int writeLine(const char *line, FILE *fp);


#endif