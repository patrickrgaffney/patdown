/* 
 * parsers.c -- markdown parsing methods
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 07/12/2016.
 * 
 *********ultrapatbeams*/

#include <string.h>
#include <ctype.h>
#include "parsers.h"
#include "markdown.h"


/******************************************************************
 * count_indentation() -- count leading indentation on a string
 * 
 * char *s -- string whose indentation to be counted
 *
 * @return -- number of leading WS chars in *s
 ******************************************************************/
static size_t count_indentation(char *s)
{
    size_t i = 0;
    while (s[i] == ' ') i++;
    return i;
}


/******************************************************************
 * static global variables -- used to hold state information
 *
 * mdblock_t lastBlock -- type of last parsed block
 * size_t indentation  -- count of leading spaces on a line
 * bool insideFencedCodeBlock -- are we inside a fenced code block?
 * int lastCodeTickChar -- last fence char used for a code block
 ******************************************************************/
static mdblock_t lastBlock = UNKNOWN;
static size_t indentation  = 0;
static bool insideFencedCodeBlock = false;


/******************************************************************
 * block_parser() -- determine which parsing function to call
 * 
 * char *line -- line read from input file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *block_parser(string_t *line)
{
    size_t i = indentation = count_indentation(line->string);
    markdown_t *node = NULL;

    if (insideFencedCodeBlock) node = parse_fenced_code_block(line);
    else if (*(line->string) == '\0') node = parse_blank_line(line);
    else if (i > 3) node = parse_indented_code_block(line);

    while (line->string[i] != '\0' && !node) {
        switch (line->string[i]) {
            case '#': node = parse_atx_header(line);
                      break;
            case '-': node = parse_horizontal_rule(line);
                      break;
            case '*': node = parse_horizontal_rule(line);
                      break;
            case '_': node = parse_horizontal_rule(line);
                      break;
            case '=': node = parse_setext_header(line);
                      break;
            case '~':
            case '`': node = parse_fenced_code_block(line);
                      break; 
            default:  break;
        }
        if (!node && isalpha(line->string[i++])) node = parse_paragraph(line);
    }
    if (!node) node = init_markdown(line, 0, line->len, PARAGRAPH);
    lastBlock = node->type;
    return node;
}


/******************************************************************
 * parse_atx_header(char *s) -- parse for an atx header
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_atx_header(string_t *s)
{
    size_t i = indentation, hashes = 0, trailing = 0, j = s->len;
    mdblock_t type;
    
    if (i > 3) return NULL;
    
    while (s->string[i] == '#') {
        hashes++;
        i++;
    }
    if (hashes < 1 || hashes > 6 || s->string[i] != ' ') {
        return init_markdown(s, i - hashes, j, PARAGRAPH);
    }
    
    // 1-unlimited spaces between hashes and first word
    while (s->string[i] == ' ') i++;
    
    // remove all spaces at end of string
    while (s->string[j] == ' ') j--;
    
    // remove all trailing hashes if they are followed by 1-inf spaces
    while (s->string[j] == '#') {
        j--;
        trailing++;
    }
    if (s->string[j] != ' ') j += trailing;
    else {
        while (s->string[j] == ' ') j--;
    }
    
    switch (hashes) {
        case 1: type = ATX_HEADER_1; break;
        case 2: type = ATX_HEADER_2; break;
        case 3: type = ATX_HEADER_3; break;
        case 4: type = ATX_HEADER_4; break;
        case 5: type = ATX_HEADER_5; break;
        case 6: type = ATX_HEADER_6; break;
        default: break;
    }
    return init_markdown(s, i, j, type);
}


/******************************************************************
 * parse_horizontal_rule() -- parse for a horizontal rule
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_horizontal_rule(string_t *s)
{
    size_t i = indentation, numChars = 0;
    int hrChar = (s->string[i] == '*' || s->string[i] == '_' || s->string[i] == '-') ? s->string[i] : -1;
    
    if (i > 3) return NULL;
    
    if (hrChar == -1) return NULL;
    else if (hrChar == '-' && lastBlock == PARAGRAPH) {
        return parse_setext_header(s);
    }
    
    // *n* number of spaces and *n* number of hrChar's
    while (s->string[i] == ' ' || s->string[i] == hrChar) {
        if (s->string[i++] == hrChar) numChars++;
    }
    
    if (s->string[i] != '\0' || numChars < 3) {
        return parse_paragraph(s);
    }
    return init_markdown(NULL, 0, 0, HORIZONTAL_RULE);
}


/******************************************************************
 * parse_paragraph() -- parse for a paragraph
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_paragraph(string_t *s)
{
    size_t i = indentation;
    size_t maxIndent = (lastBlock == PARAGRAPH) ? 10000 : 3;

    if (i > maxIndent) return NULL;

    return init_markdown(s, i, s->len, PARAGRAPH);
}


/******************************************************************
 * parse_setext_header() -- parse for a setext header
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_setext_header(string_t *s)
{
    size_t i = indentation, numChars = 0;
    int setextChar;
    if (lastBlock != PARAGRAPH) return NULL;
    
    setextChar = (s->string[i] == '-' || s->string[i] == '=') ? s->string[i] : -1;
    if (setextChar == -1) return NULL;
    
    // *n* number of setextChar's
    while (s->string[i] == setextChar) {
        numChars++;
        i++;
    }
    
    // *n* number of spaces
    while (s->string[i] == ' ') i++;
    
    if (s->string[i] != '\0' || numChars < 1) return NULL;

    if (setextChar == '=') {
        return init_markdown(NULL, 0, 0, SETEXT_HEADER_1);
    }
    else return init_markdown(NULL, 0, 0, SETEXT_HEADER_2);
}


/******************************************************************
 * parse_indented_code_block() -- parse for an indented code block
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_indented_code_block(string_t *s)
{
    if (lastBlock == PARAGRAPH) return parse_paragraph(s);
    size_t i = indentation;

    if (s->string[i] == '\0') return parse_blank_line(s);
    else if (i < 4) return NULL;
    
    return init_markdown(s, 4, s->len, INDENTED_CODE_BLOCK);
}


/******************************************************************
 * parse_blank_line() -- parse for a blank line
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_blank_line(string_t *s)
{
    size_t i = indentation;
    
    if (s->string[i] != '\0') return NULL;

    // preserve blank lines inside an INDENTED_CODE_BLOCK
    if (lastBlock == INDENTED_CODE_BLOCK && i >= 4) {
        return init_markdown(s, 4, s->len, INDENTED_CODE_BLOCK);
    }
    else return init_markdown(NULL, 0, 0, BLANK_LINE);
}


/******************************************************************
 * parse_fenced_code_block() -- parse for a fenced code block
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_fenced_code_block(string_t *s)
{
    static int lastFenceChar = 0; // Last char used for a fence
    static size_t lastFenceLen = 0; // Length of last fence
    size_t i = indentation, ticks = -i, start = 0;
    int fence = (s->string[i] == '~' || s->string[i] == '`') ? s->string[i] : -1;;
    
    while (s->string[i] == fence) i++;
    if ((ticks += i) < 3 && !insideFencedCodeBlock) return false;
    while (s->string[i] == ' ') i++;
    
    // When inside a block check for the end, else just append to block
    if (insideFencedCodeBlock) {
        if (s->string[i] != '\0' || lastFenceChar != fence || lastFenceLen != ticks) {
            return init_markdown(s, 0, s->len, FENCED_CODE_BLOCK);
        }
        else insideFencedCodeBlock = false;
        return init_markdown(NULL, 0, 0, FENCED_CODE_BLOCK_END);
    }
    else {
        // accept an info string
        if (isalpha(s->string[i])) {
            start = i;
            while (isalpha(s->string[i])) i++;
        }
        lastFenceChar = fence;
        lastFenceLen  = ticks;
        insideFencedCodeBlock = true;
        if (start > 0) {
            return init_markdown(s, start, --i, FENCED_CODE_BLOCK_START);
        }
        else return init_markdown(NULL, 0, 0, FENCED_CODE_BLOCK_START);
    }
}