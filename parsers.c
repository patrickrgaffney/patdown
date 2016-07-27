/* 
 * parsers.c -- markdown parsing methods
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 07/21/2016.
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
 ******************************************************************/
static mdblock_t lastBlock = UNKNOWN;
static size_t indentation  = 0;
static bool insideFencedCodeBlock = false;
static bool insideHTMLComment     = false;


/******************************************************************
 * ready_parser() -- get the parser ready for this line of input
 * 
 * char *line -- line read from input file
 *
 * @return -- a complete markdown_t node or NULL
 ******************************************************************/
static markdown_t *ready_parser(string_t *line)
{
    size_t i = indentation = count_indentation(line->string);
    markdown_t *node = NULL;
    
    if (insideFencedCodeBlock) {
        node = parse_fenced_code_block(line);
    }
    else if (lastBlock == HTML_BLOCK) {
        node = parse_html_block(line);
    }
    else if (*(line->string) == '\0') {
        node = parse_blank_line(line);
    }
    else if (i > 3) {
        node = parse_indented_code_block(line);
    }
    return node;
}


/******************************************************************
 * block_parser() -- determine which parsing function to call
 * 
 * char *line -- line read from input file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *block_parser(string_t *line)
{
    markdown_t *node = ready_parser(line);
    size_t i = indentation;

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
            case '<': node = parse_html_block(line);
                      break;
            default:  break;
        }
        if (!node && isalpha(line->string[i++])) {
            node = parse_paragraph(line);
        }
    }
    if (!node) node = init_markdown(line, 0, line->len - 1, PARAGRAPH);
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
    size_t i = indentation, hashes = 0, trailing = 0, j = s->len - 1;
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

    return init_markdown(s, i, s->len - 1, PARAGRAPH);
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
    
    return init_markdown(s, 4, s->len - 1, INDENTED_CODE_BLOCK);
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
        return init_markdown(s, 4, s->len - 1, INDENTED_CODE_BLOCK);
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
            return init_markdown(s, 0, s->len - 1, FENCED_CODE_BLOCK);
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


/******************************************************************
 * match_html_element() -- determine if tag matches an HTML element
 *
 * const char *e -- tag that was parsed from input
 *
 * @return -- true if a match was found, false otherwise
 ******************************************************************/
static bool match_html_element(const char *e, const size_t len)
{
    static char *len2Elements[6] = {"dd", "dl", "dt", "td", "th", "tr"};
    static char *len3Elements[4] = {"col", "dir", "div", "nav"};
    static char *len4Elements[9] = {
        "base", "body", "form", "head", "html", "link", "main", "menu", "meta"
    };
    static char *len5Elements[10] = {
        "aside", "frame", "param", "style", "table", "tbody", "tfoot", 
        "thead", "title", "track"
    };
    static char *len6Elements[10] = {
        "center", "dialog", "figure", "footer", "header", "iframe", "legend",
        "option", "script", "source"
    };
    static char *bigElements[14] = {
        "address", "article", "basefont", "caption", "colgroup", "details",
        "fieldset", "figcaption", "frameset", "menuitem", "noframes", 
        "optgroup", "section", "summary"
    };
    
    if (len == 0) return NULL;
    else if (len == 2) {
        for (size_t i = 0; i < 6; i++) {
            if (strcmp(e, len2Elements[i]) == 0) return true;
        }
        return false;
    }
    else if (len == 3) {
        for (size_t i = 0; i < 4; i++) {
            if (strcmp(e, len3Elements[i]) == 0) return true;
        }
        return false;
    }
    else if (len == 4) {
        for (size_t i = 0; i < 9; i++) {
            if (strcmp(e, len4Elements[i]) == 0) return true;
        }
        return false;
    }
    else if (len == 5) {
        for (size_t i = 0; i < 10; i++) {
            if (strcmp(e, len5Elements[i]) == 0) return true;
        }
        return false;
    }
    else if (len == 6) {
        for (size_t i = 0; i < 10; i++) {
            if (strcmp(e, len6Elements[i]) == 0) return true;
        }
        return false;
    }
    else {
        for (size_t i = 0; i < 14; i++) {
            if (strcmp(e, bigElements[i]) == 0) return true;
        }
        return false;
    }
    return false;
}


/******************************************************************
 * parse_html_block() -- parse for an HTML block (or comment)
 *
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_html_block(string_t *s)
{
    size_t i = indentation, j = 0;
    char element[15];
    
    if (i > 3 && lastBlock != HTML_BLOCK && !insideHTMLComment)
    {
        return NULL;
    }
    else if (s->string[i] == '\0') return parse_blank_line(s);
    else {
        if (lastBlock == HTML_BLOCK) {
            return init_markdown(s, 0, s->len - 1, HTML_BLOCK);
        }
        else if (s->string[i++] != '<') return NULL;
        
        // get the name of the element
        while (isalpha(s->string[i]) || isdigit(s->string[i])) {
            element[j++] = s->string[i++];
        }
        element[j] = '\0';
        
        if (match_html_element(element, j)) {
            return init_markdown(s, 0, s->len - 1, HTML_BLOCK);
        }
        else if (s->string[i] == '!') {
            return parse_html_comment(s);
        }
        else return NULL;
    }
}


/******************************************************************
 * parse_html_comment() -- parse for an HTML comment
 *
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
markdown_t *parse_html_comment(string_t *s)
{
    size_t i = indentation;
    
    if (insideHTMLComment) {
        // if inside a comment, check for its ending tag
        while (s->string[i] != '-') i++;
        
        if (s->string[i] == '-' && s->string[i + 1] == '-' && s->string[i + 2] == '>') {
            insideHTMLComment = false;
            return init_markdown(NULL, 0, 0, HTML_COMMENT);
        }
        else return NULL;
    }
    else {
        // not inside a comment, check for its starting tag
        if (s->string[i++] == '<') {
            if (s->string[i++] == '!') {
                if (s->string[i] == '-' && s->string[i + 1] == '-') {
                    insideHTMLComment = true;
                    return init_markdown(NULL, 0, 0, HTML_COMMENT);
                }
                else return NULL;
            }
            else return NULL;
        }
        else return NULL;
    }
}