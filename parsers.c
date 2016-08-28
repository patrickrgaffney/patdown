/* parsers.c    markdown parsing methods
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-08-27
 * 
 *******************************************************************/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "parsers.h"
#include "markdown.h"
#include "files.h"
#include "strings.h"
#include "block_types.h"



/*******************************************************************
 * @section External Parsing API
 *******************************************************************/

/*****
 * Parse an input file into markdown.
 *
 * @param   fp  An input file pointer opened for reading.
 *
 * @warning This function will return `NULL` if the file pointer has
 *          not be opened.
 *
 * @return  A queue of parsed, markdown nodes.
 *******************************************************************/
markdown_t *markdown(FILE *fp)
{
    markdown_t *head = NULL, *tail = NULL, *temp = NULL;
    if (!fp) return NULL;
    
    while (true) {
        temp = block_parser(fp);
        if (temp) {
            insert_markdown_queue(&head, &tail, temp);
        }
        else break;
    }
    return head;
}

/*******************************************************************
 * @section Generic Helper Functions
 *******************************************************************/

/*****
 * Count the leading spaces on a string.
 *
 * @param   s   The string on which to operate.
 *
 * @return  The amount of indentation as an integer
 *******************************************************************/
static size_t count_indentation(const char *s)
{
    size_t i = 0;
    while (s[i] == ' ') i++;
    return i;
}


/*******************************************************************
 * @section State Information and Manipulation
 *
 * @gvar    lastBlock   Type of the last block parsed.
 * @gvar    indentation Indentation of the current line.
 * @gvar    line        The actual raw string pulled from file.
 * @gvar    ready_node  An *already parsed* node.
 *******************************************************************/
static mdblock_t lastBlock = UNKNOWN;
static size_t indentation  = 0;
static string_t *line      = NULL;
static markdown_t *ready_node = NULL;


/* save or free the current line when updating state variables */
#define FREE_LINE true
#define KEEP_LINE false


/*****
 * Update the state variables.
 *
 * @param   lineAction  should we free the global `line`?
 * @param   last        type of the last parsed markdown block
 *******************************************************************/
static void update_state(const bool lineAction, const mdblock_t last)
{
    indentation = 0;
    if (lineAction) {
        free_stringt(line);
        line = NULL;
    }
    lastBlock = last;
}


/*******************************************************************
 * @section Block Parsing Functions
 *
 * If the function is passed a file pointer `*fp`, then it will
 * consume raw lines of input until it is satisfied that the block
 * has been exited.
 *
 * Each of the parsing functions crawl the raw string using indexes
 * to determine where the actual text, or *content*, begins and ends,
 * then create a substring of the raw string using those indexes.
 *
 * Each type of `mdblock_t` has its own parsing function.
 *
 * @todo    Add the various constraints of each block type to the
 *          function that parses that type -- ala: an indented code
 *          block must begin with at minimum 4 space, etc.
 *******************************************************************/
static markdown_t *parse_paragraph(FILE *fp);
static markdown_t *parse_setext_header(void);
static markdown_t *parse_blank_line(void);
static markdown_t *parse_atx_header(void);
static markdown_t *parse_horizontal_rule(void);
static markdown_t *parse_indented_code_block(FILE *fp);
static markdown_t *parse_fenced_code_block(FILE *fp);


/*****
 * Determine which parsing function to call.
 *
 * @param   fp  An input file pointer opened for reading.
 *
 * @return  A parsed markdown node or NULL if EOF is reached.
 *******************************************************************/
markdown_t *block_parser(FILE *fp)
{
    markdown_t *node = NULL;
    
    // Use the ready_node if available, otherwise read line from file
    if (ready_node) {
        node = ready_node;
        ready_node = NULL;
        return node;
    }
    else if (!line) {
        if (!(line = read_line(fp))) return NULL;
    }
    
    indentation = count_indentation(line->string);
    
    if (indentation > 3 && lastBlock != PARAGRAPH) {
        node = parse_indented_code_block(fp);
    }
    else if (*(line->string) == '\0') {
        node = parse_blank_line();
    }
    else {
        switch (line->string[indentation]) {
            case '#': 
                node = parse_atx_header(); break;
            case '-': 
            case '*': 
            case '_': 
                node = parse_horizontal_rule(); break;
            case '~':
            case '`': 
                node = parse_fenced_code_block(fp); break;
            // case '<': node = parse_html_block(line); break;
            default: break;
        }
        if (!node) node = parse_paragraph(fp);
    }
    return node;
}


/*****
 * Attempt to parse a paragrah.
 *
 * @param   fp  An input file pointer opened for reading.
 *
 * @warning If `line` is determined to be a paragraph, this function
 *          will consume all lines until the paragraph block is
 *          determined to be closed. This functionality can be turned
 *          off by passing `NULL` as the input file pointer.
 *
 * @return  NULL if not a paragraph, otherwise a `markdown_t` node.
 *******************************************************************/
static markdown_t *parse_paragraph(FILE *fp)
{
    markdown_t *node = NULL, *temp = NULL;
    size_t i = indentation;
    size_t maxIndent = (lastBlock == PARAGRAPH) ? 100 : 3;
    
    // max indentation of a paragraph is unlimited if the last line
    // was a paragraph, otherwise becomes a code block at 4 WS chars
    if (i > maxIndent) return NULL;
    
    // TODO: Check for line break around here.
    node = init_markdown(line, i, line->len - 1, PARAGRAPH);
    update_state(FREE_LINE, PARAGRAPH);
    
    // check for lazy continuation and setext headers, but only if we
    // were passed a valid input file -- otherwise just return the node
    while (fp) {
        if (!(line = read_line(fp))) break;
        indentation = count_indentation(line->string);
        
        if ((temp = parse_setext_header()) != NULL) {
            // Change the type of the previous PARAGRAPH
            node->type = temp->type;
            free_markdown(temp);
            update_state(FREE_LINE, node->type);
        }
        else if ((temp = block_parser(NULL)) != NULL) {
            if (temp->type != PARAGRAPH) {
                ready_node = temp;
                break;
            }

            // Append the new PARAGRAPH to the previous PARAGRAPH
            node->value = combine_strings("%s %s", node->value, temp->value);
            free_markdown(temp);
            update_state(FREE_LINE, PARAGRAPH);
        }   
    }
    return node;
}


/*****
 * Attempt to parse a setext header.
 *
 * This function is *only* called by `parse_paragraph()`. In fact,
 * it is called everytime we close a `PARAGRAPH` in order to
 * determine that it was not a setext header.
 *
 * @return  NULL if not a header, otherwise a `markdown_t` node.
 *******************************************************************/
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


/*****
 * Attempt to parse a blank line.
 *
 * @return  NULL if `line` isn't blank, or a `markdown_t` node.
 *******************************************************************/
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


/*****
 * Attempt to parse an ATX header.
 *
 * @return  NULL if not a header, otherwise a `markdown_t` node.
 *******************************************************************/
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


/*****
 * Attempt to parse a horizontal rule.
 *
 * @return  NULL if not a `<hr>`, otherwise a `markdown_t` node.
 *******************************************************************/
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


/*****
 * Attempt to parse an indented code block.
 *
 * @param   fp  An input file pointer opened for reading.
 *
 * @warning If `line` is determined to be a code block, this 
 *          function will consume all lines until the code block is 
 *          determined to be closed.
 *
 * @return  NULL if not a code block, otherwise a `markdown_t` node.
 *******************************************************************/
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


/*****
 * Attempt to parse a fenced code block.
 *
 * @param   fp  An input file pointer opened for reading.
 *
 * @warning If `line` is determined to be a code block, this 
 *          function will consume all lines until the code block is 
 *          determined to be closed.
 *
 * @return  NULL if not a code block, otherwise a `markdown_t` node.
 *******************************************************************/
static markdown_t *parse_fenced_code_block(FILE *fp)
{
    markdown_t *node = NULL;        // node to be returned
    md_code_block_t *info = NULL;   // used to hold info string
    size_t openFenceLength = 0, closeFenceLength = 0;
    size_t i = indentation;         // index used to create substring
    size_t k = 0;                   // index used to create info string
    int fenceChar = -1;             // character used for the fence
    int c = line->string[i];        // the first opening fence character
    
    fenceChar = (c == '~' || c == '`') ? c : -1;

    // count the number of fence characters
    while (line->string[i] == fenceChar) {
        i++;
        openFenceLength++;
    }
    if (openFenceLength < 3) return NULL;
    
    // create an empty code block node -- the actual *code block* 
    // will be added as we parse it line-by-line
    node = init_markdown(NULL, 0, 0, FENCED_CODE_BLOCK);
    
    // unlimited number of spaces after the opening fence
    while (line->string[i] == ' ') i++;

    // check for an info string
    if (isalpha(line->string[i])) {
        info = alloc_code_block_data();
        
        while (isalpha(line->string[i]) && k < 20) {
            info->lang[k++] = line->string[i++];
        }
        node->data = info;
    }
    
    // parse every line as part of this code block until the closing fence
    while (true) {
        if (!(line = read_line(fp))) break;
        i = indentation = count_indentation(line->string);
        
        // check for a code fence
        while (line->string[i] == fenceChar) {
            i++;
            closeFenceLength++;
        }
        if (openFenceLength == closeFenceLength) break;
        
        // combine strings with newline if node->value has a string value
        if (node->value->len != 0) {
            node->value = combine_strings("%s\n%s", node->value, line);
        }
        
        // otherwise just assign node->value to be a copy of `line`
        else node->value = create_substring(line, 0, line->len - 1);
        
        update_state(FREE_LINE, FENCED_CODE_BLOCK);
    }
    update_state(FREE_LINE, FENCED_CODE_BLOCK);
    printf("\n\ninfo = \'%s\'\n\n", ((md_code_block_t *)node->data)->lang);
    return node;
}


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