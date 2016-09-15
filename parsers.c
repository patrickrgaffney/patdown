/*****
 * parsers.c -- markdown parsing methods
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-06-15
 * @modified    2016-08-28
 * 
 ************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "parsers.h"
#include "markdown.h"
#include "files.h"
#include "strings.h"
#include "block_types.h"



/************************************************************************
 * @section External Parsing API
 ************************************************************************/

static markdown_t *block_parser(FILE *fp);

/**
 * markdown(fp) -- parse an input file into markdown.
 **
 * This is the external API for the parser.
 **
 * @return  A queue of nodes or NULL if file cannot be opened.
 ************************************************************************/
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

/************************************************************************
 * @section Generic Helper Functions
 ************************************************************************/

/**
 * count_indentation(s) -- count the leading indenetation on s
 **
 * @return -- amount of indentation as an integer.
 ************************************************************************/
static size_t count_indentation(const char *s)
{
    size_t i = 0;
    while (s[i] == ' ') i++;
    return i;
}


/**
 * match_html_block_element(e, len) -- determine if e is html5 element name
 **
 * @return -- true if a match is found false otherwise
 ************************************************************************/
static bool match_html_block_element(const char *e, const size_t len)
{
    static char *elements[7][14] = {
        { "p" },
        { "dd", "dl", "dt", "td", "th", "tr" },
        { "col", "dir", "div", "nav" },
        {
            "base", "body", "form", "head", "html", "link", "main", "menu", 
            "meta"
        },
        {
            "aside", "frame", "param", "style", "table", "tbody", "tfoot",
            "thead", "title", "track"
        },
        {
            "center", "dialog", "figure", "footer", "header", "iframe", 
            "legend", "option", "script", "source"
        },
        {
            "address", "article", "basefont", "caption", "colgroup", "details",
            "fieldset", "figcaption", "frameset", "menuitem", "noframes",
            "optgroup", "section", "summary"
        }
    };
    
    if (len == 0) return NULL;
    
    for (size_t i = 0; i < 14; i++) {
        if (elements[len][i] && strcmp(e, elements[len][i]) == 0) {
            return true;
        }
    }
    return false;
}


/************************************************************************
 * @section State Information and Manipulation
 ************************************************************************/
static mdblock_t lastBlock = UNKNOWN;
static size_t indentation  = 0;
static string_t *line      = NULL;
static markdown_t *ready_node = NULL;


/* Save or free the current line when updating state variables. */
#define FREE_LINE true
#define KEEP_LINE false


/**
 * update_state(lineAction, last) -- update the state variables
 *****************************************************************************/
static void update_state(const bool lineAction, const mdblock_t last)
{
    indentation = 0;
    if (lineAction) {
        free_stringt(line);
        line = NULL;
    }
    lastBlock = last;
}


/************************************************************************
 * @section Block Parsing Functions
 *
 *  If the function is passed a file pointer fp, then it will consume raw 
 *  lines of input until it is satisfied that the block has been exited.
 *
 *  Each of the parsing functions crawl the raw string using indexes to 
 *  determine where the actual text, or *content*, begins and ends, then 
 *  create a substring of the raw string using those indexes.
 *
 *  Each type of `mdblock_t` has its own parsing function.
 *
 * TODO:    Add the various constraints of each block type to the function that 
 *          parses that type -- ala: an indented code block must begin with at 
 *          minimum 4 space, etc.
 ************************************************************************/
static markdown_t *parse_paragraph(FILE *fp);
static markdown_t *parse_setext_header(void);
static markdown_t *parse_blank_line(void);
static markdown_t *parse_atx_header(void);
static markdown_t *parse_horizontal_rule(void);
static markdown_t *parse_indented_code_block(FILE *fp);
static markdown_t *parse_fenced_code_block(FILE *fp);
static markdown_t *parse_html_block(FILE *fp);
static markdown_t *parse_html_comment(FILE *fp);
static markdown_t *parse_link_reference(FILE *fp);


/**
 * block_parser(fp) -- determine which parsing function to call
 **
 * @return -- a parsed markdown node or NULL if EOF is reached
 ************************************************************************/
markdown_t *block_parser(FILE *fp)
{
    markdown_t *node = NULL;
    
    /* Use the ready_node if available, otherwise read line from file */
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
                node = parse_atx_header(); 
                break;
            case '-': 
            case '*': 
            case '_': 
                node = parse_horizontal_rule(); 
                break;
            case '~':
            case '`': 
                node = parse_fenced_code_block(fp); 
                break;
            case '<': 
                node = parse_html_block(fp); 
                break;
            case '[': 
                node = parse_link_reference(fp); 
                break;
            default: break;
        }
        if (!node) node = parse_paragraph(fp);
    }
    return node;
}


/**
 * parse_paragraph(fp) -- attempt to parse a paragrah <p> block
 **
 *  Consumes all lines until the paragraph is exited.
 **
 * @return -- NULL if not a paragraph, otherwise a markdown_t node
 ************************************************************************/
static markdown_t *parse_paragraph(FILE *fp)
{
    size_t i = indentation;
    markdown_t *node = NULL;    /* Node to be returned if PARAGRAPH.    */
    markdown_t *temp = NULL;    /* Node to hold temporary parsing data. */
    
    /* Max indentation of a paragraph is unlimited if the last line
     * was a paragraph, otherwise becomes a code block at 4 WS chars. */
    if (i > ((lastBlock == PARAGRAPH) ? 100 : 3)) return NULL;
    
    /* TODO: Check for line break around here. */
    node = init_markdown(line, i, line->len - 1, PARAGRAPH);
    update_state(FREE_LINE, PARAGRAPH);
    
    /* Check for lazy continuation and setext headers, but only if we
     * were passed a valid input file -- otherwise return the node. */
    while (fp) {
        if (!(line = read_line(fp))) break;
        indentation = count_indentation(line->string);
        
        if ((temp = parse_setext_header()) != NULL) {
            /* Change the type of the previous PARAGRAPH. */
            node->type = temp->type;
            free_markdown(temp);
            update_state(FREE_LINE, node->type);
        }
        else if ((temp = block_parser(NULL)) != NULL) {
            if (temp->type != PARAGRAPH) {
                ready_node = temp;
                break;
            }

            /* Append the new PARAGRAPH to the previous PARAGRAPH. */
            node->value = combine_strings("%s %s", node->value, temp->value);
            free_markdown(temp);
            update_state(FREE_LINE, PARAGRAPH);
        }   
    }
    return node;
}


/**
 * parse_setext_header() -- attempt to parse a setext header element
 **
 *  This function is *only* called by parse_paragraph(). In fact, it is 
 *  called everytime we close a PARAGRAPH in order to determine that it 
 *  was not a setext header.
 **
 * @return -- NULL if not a header, otherwise a markdown_t node.
 ************************************************************************/
static markdown_t *parse_setext_header(void)
{
    size_t i = indentation; 
    size_t numChars = 0;        /* Number of setext characters (=|-).   */    
    int setextChar;             /* Character used for header.           */
    int c = line->string[i];    /* First non-WS character in line.      */
    
    /* A setext header can only follow a PARAGRAPH. */
    if (lastBlock != PARAGRAPH) return NULL;
    
    setextChar = (c == '-' || c == '=') ? c : -1;
    if (setextChar == -1) return NULL;
    
    /* Parse *n* number of setextChar's. */
    while (line->string[i] == setextChar) numChars++, i++;
    
    /* Parse *n* number of spaces. */
    while (line->string[i] == ' ') i++;
    
    /* If we found any non-setextChar character, or only 
     * found one, this cannot be a setext header. */
    if (line->string[i] != '\0' || numChars < 1) return NULL;
    
    /* parse_paragraph() will update the state variables. */
    if (setextChar == '=') {
        return init_markdown(NULL, 0, 0, SETEXT_HEADER_1);
    }
    else return init_markdown(NULL, 0, 0, SETEXT_HEADER_2);
}


/**
 * parse_blank_line() -- attempt to parse a blank line element
 **
 * @return -- NULL if line isn't blank, or a markdown_t node
 ************************************************************************/
static markdown_t *parse_blank_line(void)
{
    size_t i = indentation;
    
    while (line->string[i] == ' ') i++;

    /* Cannot have any non-WS chars on a blank line. */
    if (line->string[i] != '\0') return NULL;
    else {
        update_state(FREE_LINE, BLANK_LINE);
        return init_markdown(NULL, 0, 0, BLANK_LINE);
    }
}


/**
 * parse_atx_header() -- attempt to parse an ATX header element
 **
 * @return -- NULL if not a header, otherwise a markdown_t node
 ************************************************************************/
static markdown_t *parse_atx_header(void)
{
    size_t i = indentation;
    size_t hashes = 0;              /* Number of leading hashes.        */
    size_t trailing = 0;            /* Number of trailing hashes.       */
    size_t end = line->len - 1;     /* Index of last character in line. */
    mdblock_t type;                 /* ATX_HEADING_{1-6}                */
    markdown_t *node = NULL;        /* Node to be returned.             */

    if (i > 3) return NULL;

    /* Count the number of leading hashes. */
    while (line->string[i] == '#') hashes++, i++;
    
    /* Required space after initial hashes and beginning of heading. */
    if (hashes < 1 || hashes > 6 || line->string[i] != ' ') return NULL;

    /* 1-unlimited spaces between hashes and first word. */
    while (line->string[i] == ' ') i++;

    /* Remove all spaces at back-end of string. */
    while (line->string[end] == ' ') end--;

    /* Remove all trailing hashes *if* they are 
     * followed by 1-unlimited spaces. */
    while (line->string[end] == '#') end--, trailing++;
    
    /* Required space after trailing hashes and end of heading. */
    if (line->string[end] != ' ') end += trailing;
    
    /* If required space was there, also remove all the 
     * trailing spaces between the end of the heading and  
     * the start of the trailing hashes. */
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
    node = init_markdown(line, i, end, type);
    update_state(FREE_LINE, type);
    return node;
}


/*****
 * parse_horizontal_rule() -- attempt to parse a horizontal rule element
 **
 * @return -- NULL if not an <hr>, otherwise a markdown_t node
 ************************************************************************/
static markdown_t *parse_horizontal_rule(void)
{
    size_t i = indentation;
    size_t numChars = 0;        /* Number of <hr> characters in line.   */
    markdown_t *node = NULL;    /* Node to be returned.                 */
    int hrChar;                 /* The character used for the <hr>.     */
    int c = line->string[i];    /* The first non-WS character in line.  */
    
    /* Don't worry about potential collision of a '-' <hr> with a 
     * '-' <h2> because this is checked in parse_paragraph(). */
    hrChar = (c == '*' || c == '_' || c == '-') ? c : -1;
    if (i > 3 || hrChar == -1) return NULL;

    /* Parse *n* number of spaces and *n* number of hrChar's. */
    while (line->string[i] == ' ' || line->string[i] == hrChar) {
        if (line->string[i++] == hrChar) numChars++;
    }

    /* No other characters may occur inline. */
    if (line->string[i] != '\0' || numChars < 3) return NULL;
    
    node = init_markdown(NULL, 0, 0, HORIZONTAL_RULE);
    update_state(FREE_LINE, HORIZONTAL_RULE);
    return node;
}


/**
 * parse_indented_code_block(fp) -- attempt to parse indented code block
 **
 *  Consumes all lines until the indented code block is exited.
 **
 * @return -- NULL if not a code block, otherwise a markdown_t node
 ************************************************************************/
static markdown_t *parse_indented_code_block(FILE *fp)
{
    size_t i = indentation;
    markdown_t *node = NULL;    /* Node to be returned.                 */
    markdown_t *temp = NULL;    /* Node to hold temporary parsing data. */
    char *fmt_string;           /* Format string for combine_strings(). */

    if (lastBlock == PARAGRAPH || i < 4) return NULL;

    node = init_markdown(line, 4, line->len - 1, INDENTED_CODE_BLOCK);
    update_state(FREE_LINE, INDENTED_CODE_BLOCK);
    
    /* Continue parsing until we exit the code block -- aka reach a
     * blank line followed by the start of a different block. A blank
     * line followed by a line with 4 spaces of indentation will
     * continue (become part of) this block. */
    while (true) {
        if (!(line = read_line(fp))) break;
        indentation = count_indentation(line->string);
        
        if (indentation > 3) {
            /* Add additional newline if last raw line was empty. */
            if (lastBlock == BLANK_LINE) fmt_string = "%s\n\n%s";
            else fmt_string = "%s\n%s";

            /* This is a continutation of the code block. */
            line = create_substring(line, 4, line->len - 1);
            node->value = combine_strings(fmt_string, node->value, line);
            update_state(FREE_LINE, INDENTED_CODE_BLOCK);
        }
        else {
            /* Check for a blank line -- save it for later if found. */
            if ((temp = parse_blank_line()) != NULL) {
                update_state(FREE_LINE, BLANK_LINE);
            }
            
            /* Save parsed node for next parser() call and break out. */
            else {
                ready_node = temp;
                break;
            }
        }
    }
    return node;
}


/* Maximum length of an info string on a fenced code block. */
#define INFO_STRING_MAX 20

/**
 * parse_fenced_code_block(fp) -- attempt to parse a fenced code block
 **
 *  Consumes all lines until the indented code block is exited.
 **
 * @return -- NULL if not a code block, otherwise a markdown_t node
 ************************************************************************/
static markdown_t *parse_fenced_code_block(FILE *fp)
{
    markdown_t *node = NULL;        /* Node to be returned.             */
    md_code_block_t *info = NULL;   /* Used to hold info string.        */
    size_t openFenceLength = 0;     /* Length of opening fence.         */
    size_t closeFenceLength = 0;    /* Length of closing fence.         */
    size_t i = indentation;         /* Index used to create substring.  */
    size_t k = 0;                   /* Index used to create infostring. */
    int fenceChar = -1;             /* Character used for the fence.    */
    int c = line->string[i];        /* First opening fence character.   */

    /* Count the number of fence characters. */
    fenceChar = (c == '~' || c == '`') ? c : -1;
    while (line->string[i] == fenceChar) {
        i++;
        openFenceLength++;
    }
    if (openFenceLength < 3) return NULL;
    
    /* Create an empty code block node -- the actual *code block* 
     * content (lines) will be added as we parse it line-by-line. */
    node = init_markdown(NULL, 0, 0, FENCED_CODE_BLOCK);
    
    /* Parse an unlimited number of spaces after the opening fence. */
    while (line->string[i] == ' ') i++;

    /* Check for an info string. */
    if (isalpha(line->string[i])) {
        info = alloc_code_block_data();
        
        /* Collect INFO_STRING_MAX characters for the info-string. */
        while (isalpha(line->string[i]) && k < INFO_STRING_MAX) {
            info->lang[k++] = line->string[i++];
        }
        node->data = info;
    }
    
    /* Parse every line as part of this code block 
     * until we find the closing fence. */
    while (true) {
        if (!(line = read_line(fp))) break;
        i = indentation = count_indentation(line->string);
        
        /* Check this line for a code fence. */
        while (line->string[i] == fenceChar) {
            i++;
            closeFenceLength++;
        }
        if (openFenceLength == closeFenceLength) break;
        
        /* Combine strings with newline if node->value has a string value. */
        if (node->value->len != 0) {
            node->value = combine_strings("%s\n%s", node->value, line);
        }
        
        /* Otherwise just assign node->value to be a copy of `line`. */
        else node->value = create_substring(line, 0, line->len - 1);
        
        update_state(FREE_LINE, FENCED_CODE_BLOCK);
    }
    update_state(FREE_LINE, FENCED_CODE_BLOCK);
    return node;
}


/**
 * parse_html_block(fp) -- attempt to parse an HTML block
 **
 *  An HTML_BLOCK has the following rules, as derived from John Gruber's
 *  original Markdown spec (not the CommonMark spec):
 *
 *      - There can be *no indentation* on the line with the opening tag.
 *      - The opening tag must begin with a left-angle bracket '<'.
 *      - Immediately followed by a *valid* block-level element name.
 *      - Everything after the opening tag name will be parsed as part of 
 *        this HTML_BLOCK until a BLANK_LINE is encountered.
 *
 *  Consumes all lines until the indented code block is exited.
 **
 * @return -- NULL if not an HTML block, otherwise a markdown_t node
 ************************************************************************/
static markdown_t *parse_html_block(FILE *fp)
{
    size_t i = indentation;
    markdown_t *node = NULL;    /* Node to be returned.                 */
    markdown_t *temp = NULL;    /* Node to hold temporary parsing data. */
    size_t j = 0;               /* Index used for element[].            */
    char element[15];           /* Opening elements name.               */

    /* HTML block start tag must not be indented. */
    if (i > 0) return NULL;
    
    /* Check for opening angle-bracket. */
    if (line->string[i++] != '<') return NULL;
    
    /* If it appears to be a comment, attempt to parse one. */
    if (line->string[i] == '!') return parse_html_comment(fp);
    
    /* Validate the opening tag name. */
    while (isalpha(line->string[i])) {
        element[j++] = line->string[i++];
    }
    if (!match_html_block_element(element, --j)) return NULL;
    
    node = init_markdown(line, 0, line->len - 1, HTML_BLOCK);
    update_state(FREE_LINE, HTML_BLOCK);
    
    /* Append lines to this HTML_BLOCK until a newline is encountered. */
    while (true) {
        if (!(line = read_line(fp))) break;
        i = indentation = count_indentation(line->string);
        
        /* Check for exit-condition: a BLANK_LINE was encountered. */
        if ((temp = parse_blank_line()) != NULL) {
            ready_node = temp;
            update_state(FREE_LINE, BLANK_LINE);
            break;
        }
        
        /* Otherwise, append this line to the HTML_BLOCK node. */
        node->value = combine_strings("%s\n%s", node->value, line);
    }
    return node;
}


/**
 * parse_html_comment(fp) -- attempt to parse an HTML comment
 **
 *  An HTML_COMMENT has the following rules:
 *
 *      - There can be *no indentation* on the line with the 
 *        opening sequence.
 *      - The opening sequence is: "<!--".
 *      - After the opening tag, all lines are parsed as an 
 *        HTML_COMMENT until the closing seqeunce is encountered.
 *      - The closing sequence is: "-->".
 *
 *  All content parsed while inside a HTML_COMMENT will *not* be 
 *  saved -- as we won't need it again -- they are basically ignored. 
 *  The function will still return a valid markdown_t node, just one 
 *  without any content.
 *
 *  This function is *only* called by parse_html_block().
 *
 *  Consumes all lines until the indented code block is exited.
 **
 * @return -- NULL if not an HTML comment, otherwise a markdown_t node
 ************************************************************************/
static markdown_t *parse_html_comment(FILE *fp)
{
    size_t i = indentation;

    /* Check for *required* opening sequence: "<!--". */
    if (line->string[i++] != '<') return NULL;
    if (line->string[i++] != '!') return NULL;
    if (line->string[i++] != '-') return NULL;
    if (line->string[i++] != '-') return NULL;
    
    /* Consume all lines until we find the ending sequence: "-->". */
    while (true) {
        /* Check the current line for the ending sequence. */
        while (line->string[i] != '-') i++;
        
        if (line->string[i++] == '-' && 
            line->string[i++] == '-' &&
            line->string[i]   == '>') break;
        
        /* Get the next line to check for ending sequence. */
        else {
            update_state(FREE_LINE, HTML_COMMENT);
            if (!(line = read_line(fp))) break;
            i = indentation = count_indentation(line->string);
        }
    }
    update_state(FREE_LINE, HTML_COMMENT);
    return init_markdown(NULL, 0, 0, HTML_COMMENT);
}

/**
 * parse_link_reference(fp) -- attempt to parse a link reference definition
 **
 *  A link reference definition is parsed exactly as described in the 
 *  CommonMark spec **save for one exception** -- the title may not 
 *  contain any newlines. You can place the link, destination, and title
 *  on three separate lines, but each of the three individual pieces
 *  must be on their *own* respective line.
 **
 * @return -- NULL if not a link ref def, otherwise a markdown_t node
 ************************************************************************/
static markdown_t *parse_link_reference(FILE *fp)
{
    size_t i = indentation;
    size_t j = 0;               /* Index for the link_ref_t members.    */
    markdown_t *node = NULL;    /* Node to be returned.                 */
    link_ref_t *link = NULL;
    
    /* Link definition cannot be indented more than 3 spaces. */
    if (i > 3) return NULL;
    
    /* Link label must begin with an open bracket. */
    if (line->string[i++] != '[') return NULL;
    
    link = alloc_link_reference_data();
    
    /* Pull the link label out of the definition. */
    while (isalnum(line->string[i]) || line->string[i] == '_') {
        link->link[j++] = line->string[i++];
    }
    
    /* Link label must end with a closing bracket. */
    if (line->string[i++] != ']') return NULL;
    
    /* Followed by a colon -- no colon means this is a *reference*, not
     * a definition -- so we parse it as a paragraph. */
    if (line->string[i++] != ':') return parse_paragraph(fp);
    
    /* Parse an unlimited number of whitespace characters. */
    while (line->string[i] == ' ') i++;
    
    /* Check for an optional line-ending -- possibly get another line. */
    if (!line->string[i]) {
        update_state(FREE_LINE, LINK_REFERENCE_DEF);
        if (!(line = read_line(fp))) return NULL;
        i = indentation = count_indentation(line->string);
    }
    
    /* Pull the link destination out of the definition. */
    j = 0;
    while (isgraph(line->string[i])) {
        link->dest[j++] = line->string[i++];
    }
    
    /* Parse an unlimited number of whitespace characters. */
    while (line->string[i] == ' ') i++;
    
    /* Check for an optional line-ending -- possible get another line. */
    if (!line->string[i]) {
        update_state(FREE_LINE, LINK_REFERENCE_DEF);
        if (!(line = read_line(fp))) goto return_node;
        i = indentation = count_indentation(line->string);
    }
    
    /* Link title must begin with quotation mark. */
    if (line->string[i++] != '\"') goto return_node;

    /* Pull the link title out of the definition. */
    j = 0;
    while (line->string[i] && line->string[i] != '\"') {
        link->title[j++] = line->string[i++];
    }

    goto return_node;
    
    return_node: {
        update_state(FREE_LINE, LINK_REFERENCE_DEF);
        node = init_markdown(NULL, 0, 0, LINK_REFERENCE_DEF);
        node->data = link;
        return node;
    }
}