/*****
 * parsers.c -- markdown parsing methods
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2016-10-14
 *  project:    patdown
 * 
 ************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <stdio.h>

#include "links.h"
#include "markdown.h"
#include "parsers.h"
#include "strings.h"

/** The base-size in bytes of a block buffer. **/
#define BLK_BUF 256

/** The byte-length of a newline, NULL-character. **/
#define NEWLINE 1
#define NULL_CHAR 1

/** When calling the block-functions, we can either parse the 
 ** block -- call add_markdown() -- or just check the syntax. */
#define PARSE_BLK true
#define CHK_SYNTX false

/** Block parsing prototypes. **/
static bool    block_parser(String *);
static ssize_t is_blank_line(uint8_t *, bool);
static bool    is_still_paragraph(uint8_t *);
static ssize_t parse_paragraph(uint8_t *);
static ssize_t is_atx_header(uint8_t *, bool);
static ssize_t parse_atx_header(uint8_t *, size_t, size_t);
static ssize_t is_horizontal_rule(uint8_t *, bool);
static ssize_t is_setext_header(uint8_t *);

/************************************************************************
 * External Parsing API
 ************************************************************************/

/*****
 * Call upon the parsers and generate the Markdown queue.
 *
 *  This function is the external API for the parser. Call it with 
 *  a bag of bytes and it will parse the input.
 *
 *  If there is an error while parsing -- and the error is non-
 *  fatal -- it will be displayed as a dialog to stdout.
 *
 * ARGUMENTS
 *  bytes   A string of raw bytes read from the input file.
 *
 * RETURNS
 *  true if the parsing completed, false if no nodes were parsed.
 *****/
bool markdown(String *bytes)
{
    if (!bytes->data || bytes->length == 0) return false;
    
    if (!block_parser(bytes)) return false;
    return true;
}


/************************************************************************
 * Block Parsing Functions
 ************************************************************************/

/*****
 * Parse a String of input bytes into a Markdown queue.
 *
 * ARGUMENTS
 *  bytes   A string of raw bytes read from the input file.
 *
 * RETURNS
 *  true if the parsing completed, false if no nodes were parsed.
 *****/
static bool block_parser(String *bytes)
{
    int len = 0;                    /* Length of last block. */
    uint8_t *doc = bytes->data;     /* Document pointer. */
    
    while (true) {
        size_t ws = count_indentation(doc);

        /* EOF reached? */
        if (!(*(doc + ws))) break;
        
        /* Check for blank line. */
        else if (((len = is_blank_line(doc, PARSE_BLK))) > 0) {
            doc += len;
            continue;
        }
        
        /* Switch on first non-WS character of the line. */
        switch(*(doc + ws)) {
            case '-': len = is_horizontal_rule(doc, PARSE_BLK); break;
            case '_': len = is_horizontal_rule(doc, PARSE_BLK); break;
            case '*': len = is_horizontal_rule(doc, PARSE_BLK); break;
            case '#': len = is_atx_header(doc, PARSE_BLK);      break;
            default:  len = -1;
        }
        
        /* Default to paragraph if no nodes were added. */
        if (len == -1) {
            len = parse_paragraph(doc + ws) + ws;
        }
        doc += len;
    }
    
    /* Return true only if we added at least one block to the queue. */
    if (doc == bytes->data) return false;
    return true;
}


/*****
 * Is the current line a blank line?
 *
 * ARGUMENTS
 *  data    An array of byte data (input utf8 string).
 *  parse   If true, parse the block; if false, just check syntax.
 *
 * RETURNS
 *  The size in bytes of the raw block, or -1 if not a blank line.
 *****/
static ssize_t is_blank_line(uint8_t *data, bool parse)
{
    size_t i = 0;   /* Byte-index to increment and return. */
    
    while (isblank(*data)) data++, i++;

    /* Cannot have any non-WS chars on a blank line. */
    if (*data == '\n' || !(*data)) {
        if (parse) add_markdown(NULL, BLANK_LINE, NULL);
        return i + NEWLINE;
    }
    return -1;
}


/*****
 * Is the current line a continuation of the current paragraph?
 *
 *  Check to ensure that this new line is still part of the PARAGRAPH
 *  that began on a previous line. No check is made to determine if 
 *  this line is a setext header -- that is done by default everytime 
 *  we *close* a PARAGRAPH block.
 *
 * ARGUMENTS
 *  data    An array of byte data (input utf8 string).
 *
 * RETURNS
 *  true if this is still a PARAGRAPH, false if this line belongs 
 *  to a different (*new*) block.
 *****/
static bool is_still_paragraph(uint8_t *data)
{
    /* TODO: Add checks for code fence, blockquotes, and lists. */
    return ((is_blank_line(data, CHK_SYNTX) < 0) &&
            (is_atx_header(data, CHK_SYNTX) < 0) &&
            (is_horizontal_rule(data, CHK_SYNTX) < 0));
}


/*****
 * Parse a paragraph and add it to the queue.
 *
 * ARGUMENTS
 *  data    An array of byte data (input utf8 string).
 *
 * RETURNS
 *  The size in bytes of the block that was parsed.
 *****/
static ssize_t parse_paragraph(uint8_t *data)
{
    String *p  = init_string(BLK_BUF);
    mdblock_t type = PARAGRAPH;
    set_current_block(PARAGRAPH);
    ssize_t sh = 0;     /* Length of a possible setext header. */
    
    do {
        /* Add characters until we reach a newline. */
        while (*data && *data != '\n') {
            if (p->length < p->allocd - NULL_CHAR) {
                *p->data++ = *data++;
                p->length++;
            }
            else realloc_string(p, p->allocd + BLK_BUF);
        }
        
        /* Is this next line the same paragraph? Setext header? */
        if (!is_still_paragraph(++data)) break;
        if (((sh = is_setext_header(data))) > 0) {
            if (*(data + count_indentation(data)) == '=') {
                type = SETEXT_HEADER_1;
            }
            else type = SETEXT_HEADER_2;
            break;
        }
        sh = 0;
        
        /* Add a newline and continue parsing. */
        *p->data++ = '\n';
        p->length++;
        
    } while (true);
    
    *p->data = '\0';
    p->data -= p->length;
    add_markdown(p, type, NULL);
    return p->length + sh + NEWLINE;
}

/*****
 * Is the current line an ATX header?
 *
 * ARGUMENTS
 *  data    An array of byte data (input utf8 string).
 *  parse   If true, parse the block; if false, just check syntax.
 *
 * RETURNS
 *  The size in bytes of the raw block, or -1 if not a ATX header.
 *****/
static ssize_t is_atx_header(uint8_t *data, bool parse)
{
    size_t ws = count_indentation(data);
    size_t hashes = 0;  /* Number of leading hashes. */
    size_t i = ws;      /* Byte-index to increment and return. */

    if (ws > 3) return -1;
    data += ws;

    while (*data == '#') hashes++, i++, data++;

    /* Required space after initial hashes and beginning of heading. */
    if (hashes < 1 || hashes > 6 || (*data != 0x20 && *data != '\t')) {
        return -1;
    }
    if (parse) {
        return parse_atx_header(++data, hashes, ++i);
    }
    else return ++i;
}


/*****
 * Parse an ATX header and add it to the queue.
 *
 * ARGUMENTS
 *  data    An array of byte data (input utf8 string).
 *  hashes  The number of leading hashes (determines mdblock_t).
 *  i       Index of *data for the current block (return value).
 *
 * RETURNS
 *  The size in bytes of the block that was parsed.
 *****/
static ssize_t parse_atx_header(uint8_t *data, size_t hashes, size_t i)
{
    String *h = init_string(BLK_BUF);

    /* Add characters until we reach a newline. */
    while (*data && *data != '\n') {
        if (h->length < h->allocd) {
            *h->data++ = *data++;
            h->length++;
        }
        else realloc_string(h, h->allocd + BLK_BUF);
    }
    i += h->length;

    /* Remove any trailing spaces before the newline. */
    if (*(h->data - 1) == 0x20 && h->data--) {
        while (*h->data == 0x20) h->data--, h->length--;

        /* Remove any trailing hashes if they exist. */
        while (*h->data == '#') h->data--, h->length--;
    
        /* Required space after trailing hashes and the end of the heading. */
        if (*h->data == 0x20) {
            while (*h->data == 0x20) h->data--, h->length--;
            h->data++;
        }
        /* If the required space was missing, keep the trailing hashes. */
        else if (*h->data == '#') {
            while (*h->data == '#') h->data++, h->length++;
        }
        /* Otherwise, we just removed spaces -- make room for '\0'. */
        else h->data++;
    }
    
    *h->data = '\0';
    h->data -= h->length;
    add_markdown(h, (ATX_HEADER_1 - 1) + hashes, NULL);
    return i + NEWLINE;
}


/*****
 * Is the current line a horizontal rule?
 *
 * ARGUMENTS
 *  data    An array of byte data (input utf8 string).
 *  parse   If true, parse the block; if false, just check syntax.
 *
 * RETURNS
 *  The size in bytes of the raw block, or -1 if not a horizontal rule.
 *****/
static ssize_t is_horizontal_rule(uint8_t *data, bool parse)
{
    size_t ws = count_indentation(data);
    size_t rc = 0;      /* Number of rule characters. */
    size_t i  = ws;     /* Byte-index to increment and return. */
    int8_t hr = -1;     /* Specific rule character used in this <hr>. */
    
    data += ws;
    hr = (*data == '*' || *data == '_' || *data == '-') ? *data : -1;
    
    /* Ensure that this isn't a setext header. */
    if (get_last_block() == PARAGRAPH && hr == '-') return -1;
    if (ws > 3 || hr == -1) return -1;

    /* Parse *n* number of spaces and *n* number of rule characters. */
    while (*data && (*data == 0x22 || *data == hr)) {
        if (*data++ == hr) rc++;
        i++;
    }

    /* No other characters may occur inline. */
    if ((*data == '\n' || !(*data)) && rc > 2) {
        if (parse) add_markdown(NULL, HORIZONTAL_RULE, NULL);
        return i + NEWLINE;
    }
    return -1;
}


static ssize_t is_setext_header(uint8_t *data)
{
    size_t ws = count_indentation(data);
    int8_t sc = -1;     /* Setext character used in this header. */
    size_t i  = ws;     /* Byte-index to increment and return. */
    
    data += ws;
    sc = (*data == '-' || *data == '=') ? *data : -1;
    
    /* The last (or current) block must be a paragraph. */
    if (get_last_block() != PARAGRAPH || sc == -1) return -1;

    /* Parse *n* number of consecutive setext characters. */
    while (*data && (*data == sc)) data++, i++;

    /* Parse *n* number of spaces. */
    while (*data && (*data == 0x20)) data++, i++;

   /* No other characters may occur inline. */
   if (*data == '\n' || !(*data)) {
       return i + NEWLINE;
   }
   return -1;
}


// /** is_indented_code_block() -- are we in an indented code block? *******/
// static bool is_indented_code_block(void)
// {
//     return (lastBlock != PARAGRAPH && indentation > 3);
// }
//
//
// /** parse_indented_code_block(fp) -- attempt to parse indented code block */
// static Markdown *parse_indented_code_block(FILE *fp)
// {
//     size_t i = indentation;
//     Markdown *node = NULL;      /* Node to be returned. */
//     Markdown *temp = NULL;      /* Node to hold temporary parsing data. */
//     char *fmt_string;           /* Format string for combine_strings(). */
//
//     if (!is_indented_code_block()) return NULL;
//
//     node = init_markdown(line, 4, line->len - 1, INDENTED_CODE_BLOCK);
//     update_state(FREE_LINE, INDENTED_CODE_BLOCK);
//
//     /* Continue parsing until we exit the code block (reach a blank line
//      * followed by the start of a different block). A blank line followed
//      * by a line with 4 spaces of indentation will continue this block. */
//     while (true) {
//         if (!(line = read_line(fp))) break;
//         indentation = count_indentation(line->string);
//
//         if (is_indented_code_block()) {
//             /* Add additional newline if last raw line was empty. */
//             if (lastBlock == BLANK_LINE) fmt_string = "%s\n\n%s";
//             else fmt_string = "%s\n%s";
//
//             /* This is a continutation of the code block. */
//             line = create_substring(line, 4, line->len - 1);
//             node->value = combine_strings(fmt_string, node->value, line);
//             update_state(FREE_LINE, INDENTED_CODE_BLOCK);
//         }
//         else {
//             /* Check for a blank line -- save it for later if found. */
//             if (is_blank_line()) update_state(FREE_LINE, BLANK_LINE);
//             else {
//                 update_state(KEEP_LINE, INDENTED_CODE_BLOCK);
//                 break;
//             }
//         }
//     }
//     return node;
// }
//
// /* Maximum length of an info string on a fenced code block. */
// #define INFO_STRING_MAX 20
//
// /** is_code_fence() -- is the current line a code fence? ****************/
// static bool is_code_fence(void)
// {
//     size_t i = indentation;
//     size_t fenceLen = 0;            /* Length of the fence. */
//     int fenceChar = -1;             /* Character used for the fence. */
//     int c = line->string[i];        /* First non-WS character. */
//
//     /* Count the number of fence characters. */
//     fenceChar = (c == '~' || c == '`') ? c : -1;
//     while (line->string[i] == fenceChar) i++, fenceLen++;
//
//     return (fenceLen > 2);
//
// }
//
// /** parse_fenced_code_block(fp) -- attempt to parse a fenced code block */
// static Markdown *parse_fenced_code_block(FILE *fp)
// {
//     Markdown *node = NULL;          /* Node to be returned. */
//     CodeBlock *info = NULL;         /* Used to hold info string. */
//     size_t openFenceLength = 0;     /* Length of opening fence. */
//     size_t closeFenceLength = 0;    /* Length of closing fence. */
//     size_t i = indentation;         /* Index used to create substring. */
//     size_t k = 0;                   /* Index used to create infostring. */
//     int fenceChar = -1;             /* Character used for the fence. */
//     int c = line->string[i];        /* First opening fence character. */
//
//     /* Count the number of fence characters. */
//     fenceChar = (c == '~' || c == '`') ? c : -1;
//     while (line->string[i] == fenceChar) {
//         i++;
//         openFenceLength++;
//     }
//     if (openFenceLength < 3) return NULL;
//
//     /* Create an empty code block node -- the actual *code block*
//      * content (lines) will be added as we parse it line-by-line. */
//     node = init_markdown(NULL, 0, 0, FENCED_CODE_BLOCK);
//
//     /* Parse an unlimited number of spaces after the opening fence. */
//     while (line->string[i] == ' ') i++;
//
//     /* Check for an info string. */
//     if (isalpha(line->string[i])) {
//         info = alloc_code_block_data();
//
//         /* Collect INFO_STRING_MAX characters for the info-string. */
//         while (isalpha(line->string[i]) && k < INFO_STRING_MAX) {
//             info->lang[k++] = line->string[i++];
//         }
//         node->data = info;
//     }
//
//     /* Parse every line as part of this code block
//      * until we find the closing fence. */
//     while (true) {
//         if (!(line = read_line(fp))) break;
//         i = indentation = count_indentation(line->string);
//
//         /* Check this line for a code fence. */
//         while (line->string[i] == fenceChar) i++, closeFenceLength++;
//         if (openFenceLength == closeFenceLength) break;
//
//         /* Combine strings with newline if node->value has a string value. */
//         if (node->value->len != 0) {
//             node->value = combine_strings("%s\n%s", node->value, line);
//         }
//         /* Otherwise just assign node->value to be a copy of `line`. */
//         else node->value = create_substring(line, 0, line->len - 1);
//
//         update_state(FREE_LINE, FENCED_CODE_BLOCK);
//     }
//     update_state(FREE_LINE, FENCED_CODE_BLOCK);
//     return node;
// }
// 
// /** match_html_block_element(e, len) -- determine if e is html5 element name */
// static bool match_html_block_element(const char *e, const size_t len)
// {
//     static char *elements[7][14] = {
//         { "p" },
//         { "dd", "dl", "dt", "td", "th", "tr" },
//         { "col", "dir", "div", "nav" },
//         { "base", "body", "form", "head", "html", "link", "main", "menu",
//           "meta" },
//         { "aside", "frame", "param", "style", "table", "tbody", "tfoot",
//           "thead", "title", "track" },
//         { "center", "dialog", "figure", "footer", "header", "iframe",
//           "legend", "option", "script", "source" },
//         { "address", "article", "basefont", "caption", "colgroup", "details",
//           "fieldset", "figcaption", "frameset", "menuitem", "noframes",
//           "optgroup", "section", "summary" }
//     };
//
//     if (len == 0) return false;
//
//     for (size_t i = 0; i < 14; i++) {
//         if (elements[len][i] && strcmp(e, elements[len][i]) == 0) {
//             return true;
//         }
//     }
//     return false;
// }
// 
//
//
// /** parse_html_block(fp) -- attempt to parse an HTML block **************/
// static Markdown *parse_html_block(FILE *fp)
// {
//     size_t i = indentation;
//     Markdown *node = NULL;      /* Node to be returned. */
//     Markdown *temp = NULL;      /* Node to hold temporary parsing data. */
//     size_t j = 0;               /* Index used for element[]. */
//     char element[15];           /* Opening elements name. */
//
//     /* HTML block start tag must not be indented. */
//     if (i > 0) return NULL;
//
//     /* Check for opening angle-bracket. */
//     if (line->string[i++] != '<') return NULL;
//
//     /* If it appears to be a comment, attempt to parse one. */
//     if (line->string[i] == '!') return parse_html_comment(fp);
//
//     /* Validate the opening tag name. */
//     while (isalpha(line->string[i])) {
//         element[j++] = line->string[i++];
//     }
//     if (!match_html_block_element(element, --j)) return NULL;
//
//     node = init_markdown(line, 0, line->len - 1, HTML_BLOCK);
//     update_state(FREE_LINE, HTML_BLOCK);
//
//     /* Append lines to this HTML_BLOCK until a newline is encountered. */
//     while (true) {
//         if (!(line = read_line(fp))) break;
//         i = indentation = count_indentation(line->string);
//
//         /* Check for exit-condition: a BLANK_LINE was encountered. */
//         if (is_blank_line()) {
//             update_state(FREE_LINE, BLANK_LINE);
//             break;
//         }
//
//         /* Otherwise, append this line to the HTML_BLOCK node. */
//         node->value = combine_strings("%s\n%s", node->value, line);
//     }
//     return node;
// }
//
//
// /** parse_html_comment(fp) -- attempt to parse an HTML comment **********/
// static Markdown *parse_html_comment(FILE *fp)
// {
//     size_t i = indentation;
//
//     /* Check for *required* opening sequence: "<!--". */
//     if (line->string[i++] != '<') return NULL;
//     if (line->string[i++] != '!') return NULL;
//     if (line->string[i++] != '-') return NULL;
//     if (line->string[i++] != '-') return NULL;
//
//     /* Consume all lines until we find the ending sequence: "-->". */
//     while (true) {
//         /* Check the current line for the ending sequence. */
//         while (line->string[i] != '-') i++;
//
//         if (line->string[i++] == '-' &&
//             line->string[i++] == '-' &&
//             line->string[i]   == '>') break;
//
//         /* Get the next line to check for ending sequence. */
//         else {
//             update_state(FREE_LINE, HTML_COMMENT);
//             if (!(line = read_line(fp))) break;
//             i = indentation = count_indentation(line->string);
//         }
//     }
//     update_state(FREE_LINE, HTML_COMMENT);
//     return init_markdown(NULL, 0, 0, HTML_COMMENT);
// }
//
// /** parse_link_reference(fp) -- attempt to parse a link reference definition */
// static Markdown *parse_link_ref_defs(FILE *fp)
// {
//     size_t i = indentation;
//     size_t j = 0;               /* Index for the link_ref_t members.    */
//     Markdown *node = NULL;      /* Node to be returned.                 */
//     LinkRef *ref = NULL;        /* Link information to attach to node.  */
//
//     /* Link definition cannot be indented more than 3 spaces. */
//     if (i > 3) return NULL;
//
//     /* Link label must begin with an open bracket. */
//     if (line->string[i++] != '[') return NULL;
//
//     ref = init_link_ref();
//
//     /* Pull the link label out of the definition. */
//     while (line->string[i] && line->string[i] != ']' && j < 1000) {
//         ref->label[j++] = line->string[i++];
//     }
//     ref->label[j] = '\0';
//
//     /* Link label must end with a closing bracket. */
//     if (line->string[i++] != ']') return NULL;
//
//     /* Followed by a colon -- no colon means this is a *reference*, not
//      * a definition -- so we parse it as a paragraph. */
//     if (line->string[i++] != ':') return parse_paragraph(fp);
//
//     /* Parse an unlimited number of whitespace characters. */
//     while (line->string[i] == ' ') i++;
//
//     /* Check for an optional line-ending -- possibly get another line. */
//     if (!line->string[i]) {
//         update_state(FREE_LINE, LINK_REFERENCE_DEF);
//         if (!(line = read_line(fp))) return NULL;
//         i = indentation = count_indentation(line->string);
//     }
//
//     /* Pull the link destination out of the definition. */
//     j = 0;
//     while (line->string[i] && line->string[i] != ' ' && j < 1000) {
//         ref->dest[j++] = line->string[i++];
//     }
//     ref->dest[j] = '\0';
//
//     /* Parse an unlimited number of whitespace characters. */
//     while (line->string[i] == ' ') i++;
//
//     /* Check for an optional line-ending -- possible get another line. */
//     if (!line->string[i]) {
//         update_state(FREE_LINE, LINK_REFERENCE_DEF);
//         if (!(line = read_line(fp))) goto return_node;
//         i = indentation = count_indentation(line->string);
//     }
//
//     /* Link title must begin with quotation mark. */
//     if (line->string[i++] != '\"') goto return_node;
//
//     /* Pull the link title out of the definition. */
//     j = 0;
//     while (line->string[i] && line->string[i] != '\"' && j < 1000) {
//         ref->title[j++] = line->string[i++];
//     }
//     ref->title[j] = '\0';
//     goto return_node;
//
//     return_node:
//         update_state(FREE_LINE, LINK_REFERENCE_DEF);
//         node = init_markdown(NULL, 0, 0, LINK_REFERENCE_DEF);
//         node->data = ref;
//         return node;
// }
