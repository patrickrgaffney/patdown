/* 
 * parsers.c -- markdown parsing methods
 * 
 * Created by PAT GAFFNEY on 06/15/2016.
 * Last modified on 08/24/2016.
 * 
 *********ultrapatbeams*/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "parsers.h"
#include "markdown.h"
#include "files.h"
#include "strings.h"

/*******************************************************************/
/* GENERIC HELPER FUNCTIONS ****************************************/
/*******************************************************************/

/* count_indentation(s) -- count leading indentation on s **********/
static size_t count_indentation(char *s)
{
    size_t i = 0;
    while (s[i] == ' ') i++;
    return i;
}


/*******************************************************************/
/* STATE INFORMATION & MANIPULATION ********************************/
/*******************************************************************/

/* static global variables -- used to hold state information *******/
static mdblock_t lastBlock = UNKNOWN;   // type of the last block
static size_t indentation  = 0;         // current lines indentation
static string_t *line      = NULL;      // the actual line data
static markdown_t *ready_node = NULL;   // an *already* parsed node


/* save or free the current line when leaving the parser ***********/
#define FREE_LINE true
#define KEEP_LINE false


/* update_state(bool, last) -- update the state variables **********/
static void update_state(const bool lineAction, const mdblock_t last)
{
    indentation = 0;
    if (lineAction) {
        free_stringt(line);
        line = NULL;
    }
    lastBlock = last;
}


/*******************************************************************/
/* BLOCK PARSING FUNCTIONS *****************************************/
/*******************************************************************/

/* NOTE: if the function is passed a file pointer (*fp), then it will
 * consume raw lines of input until it is satified that the block has
 * been exited.
 *
 * each of the parsing functions crawl the raw string using indexes
 * to determine where the actual text (content) begins and ends --
 * then creates a substring of the raw string using those 2 indexes.
 *******************************************************************/

/* each mdblock_t has its own parsing function *********************/
static markdown_t *parse_paragraph(FILE *fp);
static markdown_t *parse_setext_header(void);
static markdown_t *parse_blank_line(void);
static markdown_t *parse_atx_header(void);
static markdown_t *parse_horizontal_rule(void);
static markdown_t *parse_indented_code_block(FILE *fp);


/* block_parser(fp) -- determine the parsing function to call ******/
/*** NOTE: this is the external API for the parser *****************/
markdown_t *block_parser(FILE *fp)
{
    markdown_t *node = NULL;
    if (ready_node) {
        node = ready_node;
        ready_node = NULL;
        return node;
    }
    else if (!line) {
        if (!(line = read_line(fp))) return NULL;
    }
    
    size_t i = indentation = count_indentation(line->string);
    
    if (indentation > 3) node = parse_indented_code_block(fp);
    else if (*(line->string) == '\0') node = parse_blank_line();
    else {
        switch (line->string[i]) {
            case '#': node = parse_atx_header(); break;
            case '-': 
            case '*': 
            case '_': node = parse_horizontal_rule(); break;
            // case '~':
            // case '`': node = parse_fenced_code_block(line); break;
            // case '<': node = parse_html_block(line); break;
            default: break;
        }
        if (!node) node = parse_paragraph(fp);
    }
    return node;
}



/* parse_paragraph(fp) -- parse lines for a complete paragraph *****/
/*** NOTE: consumes lines until the paragraph is closed ************/
static markdown_t *parse_paragraph(FILE *fp)
{
    markdown_t *node = NULL, *temp = NULL;
    size_t i = indentation;
    size_t maxIndent = (lastBlock == PARAGRAPH) ? 10000 : 3;
    mdblock_t t;
    
    // max indentation of a paragraph is unlimited if the last line
    // was a paragraph, otherwise becomes a code block at 4 WS chars
    if (i > maxIndent) return NULL;
    
    // TODO: Check for line break around here.
    node = init_markdown(line, i, line->len - 1, PARAGRAPH);
    update_state(FREE_LINE, PARAGRAPH);
    line = read_line(fp);
    
    // check next line to see if this paragraph is a setext heading
    if ((temp = parse_setext_header()) != NULL) {
        // Change the type of the previous PARAGRAPH
        node->type = temp->type;
        free_markdown(temp);
        update_state(FREE_LINE, node->type);
    }
    
    // check next line for a paragraph (lazy) continuation
    else if ((temp = block_parser(fp)) != NULL) {
        
        t = temp->type;
        if (t == PARAGRAPH || t == SETEXT_HEADER_1 || t == SETEXT_HEADER_2) {
            // Append the new PARAGRAPH to the previous PARAGRAPH
            node->value = combine_strings("%s %s", node->value, temp->value);
            
            // Change the type of the new, combined paragraph, this allows
            // the SETEXT_HEADER_x type to *float* to the top node
            node->type  = temp->type;
            free_markdown(temp);
            update_state(FREE_LINE, PARAGRAPH);
        }
        
        // otherwise, just this will be the *next* node returned
        else ready_node = temp;
    }
    return node;
}


/* parse_setext_header() -- parse line for setext header ***********/
/*** NOTE: this function is only called by parse_paragraph() *******/
static markdown_t *parse_setext_header(void)
{
    size_t i = indentation, numChars = 0;
    int setextChar, c = line->string[i];
    
    // a setext header can only follow a paragraph
    if (lastBlock != PARAGRAPH) return NULL;
    
    setextChar = (c == '-' || c == '=') ? c : -1;
    if (setextChar == -1) return NULL;
    
    // *n* number of setextChar's
    while (line->string[i] == setextChar) numChars++, i++;
    
    // *n* number of spaces
    while (line->string[i] == ' ') i++;
    
    // if we found any non-setextChar character, or only found one,
    // this cannot be a setext header
    if (line->string[i] != '\0' || numChars < 1) return NULL;
    
    // parse_paragraph() will update the state variables
    if (setextChar == '=') {
        return init_markdown(NULL, 0, 0, SETEXT_HEADER_1);
    }
    else return init_markdown(NULL, 0, 0, SETEXT_HEADER_2);
}


/* parse_blank_line() -- parse for an empty line *******************/
static markdown_t *parse_blank_line(void)
{
    size_t i = indentation;
    
    while (line->string[i] == ' ') i++;

    if (line->string[i] != '\0') return NULL;
    else {
        update_state(FREE_LINE, BLANK_LINE);
        return init_markdown(NULL, 0, 0, BLANK_LINE);
    }
}


/* parse_atx_header() -- parse for an atx header *******************/
static markdown_t *parse_atx_header(void)
{
    size_t i = indentation;
    size_t hashes = 0, trailing = 0;    // leading & trailing hashes
    size_t end = line->len - 1;         // index of last character
    mdblock_t type;                     // ATX_HEADING_{1-6}
    markdown_t *temp = NULL;

    if (i > 3) return NULL;

    while (line->string[i] == '#') hashes++, i++;
    
    // required space after initial hashes and beginning of heading
    if (hashes < 1 || hashes > 6 || line->string[i] != ' ') {
        return NULL;
    }

    // 1-unlimited spaces between hashes and first word
    while (line->string[i] == ' ') i++;

    // remove all spaces at end of string
    while (line->string[end] == ' ') end--;

    // remove all trailing hashes if they are followed by 1-inf spaces
    while (line->string[end] == '#') end--, trailing++;
    
    // required space after trailing hashes and end of heading
    if (line->string[end] != ' ') end += trailing;
    
    // if required space, also remove all the trailing spaces between 
    // the end of the heading and the start of the trailing hashes
    else {
        while (line->string[end] == ' ') end--;
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
    temp = init_markdown(line, i, end, type);
    update_state(FREE_LINE, type);
    return temp;
}


/* parse_horizontal_rule() -- parse for a horizontal rule **********/
static markdown_t *parse_horizontal_rule(void)
{
    size_t i = indentation, numChars = 0;
    markdown_t *temp = NULL;
    int hrChar, c = line->string[i];
    hrChar = (c == '*' || c == '_' || c == '-') ? c : -1;
    
    // dont worry about potential collision of a `-` <hr> with a 
    // `-` <h2> because this is checked in parse_paragraph()
    if (i > 3 || hrChar == -1) return NULL;

    // *n* number of spaces and *n* number of hrChar's
    while (line->string[i] == ' ' || line->string[i] == hrChar) {
        if (line->string[i++] == hrChar) numChars++;
    }

    // no other characters may occur inline
    if (line->string[i] != '\0' || numChars < 3) return NULL;
    
    temp = init_markdown(NULL, 0, 0, HORIZONTAL_RULE);
    update_state(FREE_LINE, HORIZONTAL_RULE);
    return temp;
}


/* parse_indented_code_block(fp) -- parse for indented code block **/
/*** NOTE: consumes lines until the code block is closed ***********/
static markdown_t *parse_indented_code_block(FILE *fp)
{
    size_t i = indentation;
    markdown_t *node = NULL, *temp = NULL;
    string_t *tempstr = NULL;
    char *fmt_string; // used for combine_strings()

    if (lastBlock == PARAGRAPH) return NULL;
    else if (i < 4) return NULL;

    node = init_markdown(line, 4, line->len - 1, INDENTED_CODE_BLOCK);
    update_state(FREE_LINE, INDENTED_CODE_BLOCK);
    
    // continue parsed until we exit the code block
    while (true) {
        if (!(line = read_line(fp))) break;
        indentation = count_indentation(line->string);
        
        if (indentation > 3) {
            // add additional newline if last raw line was empty
            if (lastBlock == BLANK_LINE) fmt_string = "%s\n\n%s";
            else fmt_string = "%s\n%s";

            // this is a continutation of the code block
            line = create_substring(line, 4, line->len - 1);
            node->value = combine_strings(fmt_string, node->value, line);
            update_state(FREE_LINE, INDENTED_CODE_BLOCK);
        }
        else {
            // check for a blank line -- save it for later if found
            if ((temp = parse_blank_line()) != NULL) {
                update_state(FREE_LINE, BLANK_LINE);
            }
            
            // save parsed node for next parser() call and break out
            else {
                ready_node = temp;
                break;
            }
        }
    }
    return node;
}


/******************************************************************
 * parse_fenced_code_block() -- parse for a fenced code block
 * 
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
// markdown_t *parse_fenced_code_block(string_t *s)
// {
//     static int lastFenceChar = 0; // Last char used for a fence
//     static size_t lastFenceLen = 0; // Length of last fence
//     size_t i = indentation, ticks = -i, start = 0;
//     int fence = (s->string[i] == '~' || s->string[i] == '`') ? s->string[i] : -1;;
//
//     while (s->string[i] == fence) i++;
//     if ((ticks += i) < 3 && !insideFencedCodeBlock) return false;
//     while (s->string[i] == ' ') i++;
//
//     // When inside a block check for the end, else just append to block
//     if (insideFencedCodeBlock) {
//         if (s->string[i] != '\0' || lastFenceChar != fence || lastFenceLen != ticks) {
//             return init_markdown(s, 0, s->len - 1, FENCED_CODE_BLOCK);
//         }
//         else insideFencedCodeBlock = false;
//         return init_markdown(NULL, 0, 0, FENCED_CODE_BLOCK_END);
//     }
//     else {
//         // accept an info string
//         if (isalpha(s->string[i])) {
//             start = i;
//             while (isalpha(s->string[i])) i++;
//         }
//         lastFenceChar = fence;
//         lastFenceLen  = ticks;
//         insideFencedCodeBlock = true;
//         if (start > 0) {
//             return init_markdown(s, start, --i, FENCED_CODE_BLOCK_START);
//         }
//         else return init_markdown(NULL, 0, 0, FENCED_CODE_BLOCK_START);
//     }
// }


/******************************************************************
 * match_html_element() -- determine if tag matches an HTML element
 *
 * const char *e -- tag that was parsed from input
 *
 * @return -- true if a match was found, false otherwise
 ******************************************************************/
// static bool match_html_element(const char *e, const size_t len)
// {
//     static char *len2Elements[6] = {"dd", "dl", "dt", "td", "th", "tr"};
//     static char *len3Elements[4] = {"col", "dir", "div", "nav"};
//     static char *len4Elements[9] = {
//         "base", "body", "form", "head", "html", "link", "main", "menu", "meta"
//     };
//     static char *len5Elements[10] = {
//         "aside", "frame", "param", "style", "table", "tbody", "tfoot",
//         "thead", "title", "track"
//     };
//     static char *len6Elements[10] = {
//         "center", "dialog", "figure", "footer", "header", "iframe", "legend",
//         "option", "script", "source"
//     };
//     static char *bigElements[14] = {
//         "address", "article", "basefont", "caption", "colgroup", "details",
//         "fieldset", "figcaption", "frameset", "menuitem", "noframes",
//         "optgroup", "section", "summary"
//     };
//
//     if (len == 0) return NULL;
//     else if (len == 2) {
//         for (size_t i = 0; i < 6; i++) {
//             if (strcmp(e, len2Elements[i]) == 0) return true;
//         }
//         return false;
//     }
//     else if (len == 3) {
//         for (size_t i = 0; i < 4; i++) {
//             if (strcmp(e, len3Elements[i]) == 0) return true;
//         }
//         return false;
//     }
//     else if (len == 4) {
//         for (size_t i = 0; i < 9; i++) {
//             if (strcmp(e, len4Elements[i]) == 0) return true;
//         }
//         return false;
//     }
//     else if (len == 5) {
//         for (size_t i = 0; i < 10; i++) {
//             if (strcmp(e, len5Elements[i]) == 0) return true;
//         }
//         return false;
//     }
//     else if (len == 6) {
//         for (size_t i = 0; i < 10; i++) {
//             if (strcmp(e, len6Elements[i]) == 0) return true;
//         }
//         return false;
//     }
//     else {
//         for (size_t i = 0; i < 14; i++) {
//             if (strcmp(e, bigElements[i]) == 0) return true;
//         }
//         return false;
//     }
//     return false;
// }


/******************************************************************
 * parse_html_block() -- parse for an HTML block (or comment)
 *
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
// markdown_t *parse_html_block(string_t *s)
// {
//     size_t i = indentation, j = 0;
//     char element[15];
//
//     if (i > 3 && lastBlock != HTML_BLOCK && !insideHTMLComment)
//     {
//         return NULL;
//     }
//     else if (s->string[i] == '\0') return parse_blank_line(s);
//     else {
//         if (lastBlock == HTML_BLOCK) {
//             return init_markdown(s, 0, s->len - 1, HTML_BLOCK);
//         }
//         else if (s->string[i++] != '<') return NULL;
//
//         // get the name of the element
//         while (isalpha(s->string[i]) || isdigit(s->string[i])) {
//             element[j++] = s->string[i++];
//         }
//         element[j] = '\0';
//
//         if (match_html_element(element, j)) {
//             return init_markdown(s, 0, s->len - 1, HTML_BLOCK);
//         }
//         else if (s->string[i] == '!') {
//             return parse_html_comment(s);
//         }
//         else return NULL;
//     }
// }


/******************************************************************
 * parse_html_comment() -- parse for an HTML comment
 *
 * char *s -- original string read from file
 *
 * @return -- an markdown_t node or NULL
 ******************************************************************/
// markdown_t *parse_html_comment(string_t *s)
// {
//     size_t i = indentation;
//
//     if (insideHTMLComment) {
//         // if inside a comment, check for its ending tag
//         while (s->string[i] != '-') i++;
//
//         if (s->string[i] == '-' && s->string[i + 1] == '-' && s->string[i + 2] == '>') {
//             insideHTMLComment = false;
//             return init_markdown(NULL, 0, 0, HTML_COMMENT);
//         }
//         else return NULL;
//     }
//     else {
//         // not inside a comment, check for its starting tag
//         if (s->string[i++] == '<') {
//             if (s->string[i++] == '!') {
//                 if (s->string[i] == '-' && s->string[i + 1] == '-') {
//                     insideHTMLComment = true;
//                     return init_markdown(NULL, 0, 0, HTML_COMMENT);
//                 }
//                 else return NULL;
//             }
//             else return NULL;
//         }
//         else return NULL;
//     }
// }