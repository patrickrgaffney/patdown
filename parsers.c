/**
 * parsers.c: markdown parsing methods
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-06-15
 *  modified:   2017-01-06
 *  project:    patdown
 * 
 ************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <stdio.h>

#include "patdown.h"
#include "strings.h"

/** The **base-size** in bytes of a block buffer. */
#define BLK_BUF 256

/** Named constants for particular byte-lengths. */
#define NEWLINE 1
#define NULL_CHAR 1

/** Named constants for the boolean parameter of parsing functions. */
#define PARSE_BLK true
#define CHK_SYNTX false

/** The number of characters to compare when parsing html tag names. */
#define TAG_LEN 25

/* Block parsing prototypes. */
static bool    block_parser(String *);
static ssize_t is_blank_line(uint8_t *, bool);
static bool    is_still_paragraph(uint8_t *);
static ssize_t parse_paragraph(uint8_t *);
static ssize_t is_atx_header(uint8_t *, bool);
static ssize_t parse_atx_header(uint8_t *, size_t, size_t);
static ssize_t is_horizontal_rule(uint8_t *, bool);
static ssize_t is_setext_header(uint8_t *);
static ssize_t parse_indented_code_block(uint8_t *);
static ssize_t is_opening_code_fence(uint8_t *, bool);
static ssize_t is_closing_code_fence(uint8_t *, CodeBlk *);
static size_t  parse_fenced_code_block(uint8_t *, CodeBlk *, size_t);
static ssize_t is_html_block(uint8_t *, bool);
static ssize_t is_link_definition(uint8_t *, bool);
static ssize_t is_blockquote(uint8_t *data, bool parse);

/************************************************************************
 * # External Parsing API
 ************************************************************************/

/**
 * Call upon the parsers and generate the Markdown queue.
 *
 * This function is the external API for the parser. Call it with 
 * a bag of utf8 bytes and it will parse them into a queue of markdown
 * nodes that can then be represented in a different way.
 *
 * - parameter bytes: A string of raw bytes read from the input file.
 *
 * - returns: true if the parsing completed, false if no nodes 
 *   were parsed.
 */
bool markdown(String *bytes)
{
    if (!bytes->data || bytes->length == 0) return false;
    
    if (!block_parser(bytes)) return false;
    return true;
}


/************************************************************************
 * # Block Parsing Functions
 *
 * Each block has it's own parsing function -- some have more than one.
 * Any function with the name `is_<block_name>()` performs a syntax 
 * check on the current line to determine if it could be parsed as a 
 * <block_name>. Each of these functions takes a Boolean `parse`
 * parameter that determine whether or not the function should actually
 * parse the block after checking the syntax. Each of the *parsing*
 * functions have the name `parse_<block_name>()`.
 * 
 ************************************************************************/

/**
 * Parse a String of input bytes into a Markdown queue.
 *
 * - parameter bytes: A string of raw bytes read from the input file.
 *
 * - returns: true if the parsing completed, false if no nodes 
 *   were parsed.
 */
static bool block_parser(String *bytes)
{
    uint8_t *doc = bytes->data;     /* Document pointer. */
    ssize_t len  = 0;               /* Length of last block. */
    size_t total = 0;               /* Total bytes parsed. */
    
    while (true) {
        if (total >= bytes->length) break;
        
        size_t ws = count_indentation(doc);
        
        /* Check for blank line, returns 0 for EOF. */
        if (((len = is_blank_line(doc, PARSE_BLK))) > 0) {
            doc += len;
            total += len;
            continue;
        }
        
        /* Zero is returned when EOF is found. */
        else if (len == 0) break;
        
        /* Check for indented code block. */
        else if (ws > 3) {
            len = parse_indented_code_block(doc);
            doc += len;
            total += len;
            continue;
        }
        
        /* Switch on first non-WS character of the line. */
        switch(*(doc + ws)) {
            case '-': len = is_horizontal_rule(doc, PARSE_BLK);     break;
            case '_': len = is_horizontal_rule(doc, PARSE_BLK);     break;
            case '*': len = is_horizontal_rule(doc, PARSE_BLK);     break;
            case '#': len = is_atx_header(doc, PARSE_BLK);          break;
            case '`': len = is_opening_code_fence(doc, PARSE_BLK);  break;
            case '~': len = is_opening_code_fence(doc, PARSE_BLK);  break;
            case '<': len = is_html_block(doc, PARSE_BLK);          break;
            case '[': len = is_link_definition(doc, PARSE_BLK);     break;
            case '>': len = is_blockquote(doc, PARSE_BLK);          break;
            default:  len = -1;
        }
        
        /* Default to paragraph if no nodes were added. */
        if (len == -1) len = parse_paragraph(doc + ws) + ws;
        doc += len;
        total += len;
    }
    
    /* Return true only if we added at least one block to the queue. */
    return (doc != bytes->data);
}


/************************************************************************
 * ## Blank Lines
 *
 * Blank lines contain only WS characters: spaces, tabs, and a newline.
 *
 * Blank lines produce no output, but they are inserted into the Markdown
 * queue in order to keep block precedence as the parsing continues.
 *
 ************************************************************************/

/**
 * Check the next line for a blank line.
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter parse: If true, parse the block; if false, just check syntax.
 *
 * - returns: The size in bytes of the line, or -1 if not a blank line.
 */
static ssize_t is_blank_line(uint8_t *data, bool parse)
{
    size_t i = 0;   /* Byte-index to increment and return. */
    
    if (!(*data)) return 0;
    while (isblank(*data)) data++, i++;
    
    if (*data == '\n') {
        if (parse) add_markdown(NULL, BLANK_LINE, NULL);
        
        /* Don't append a newline byte if we reached EOF. */
        return !(*data) ? i : i + NEWLINE;
    }
    return -1;
}


/************************************************************************
 * ## Paragraphs
 *
 * Paragraphs are a seqeunce of non-blank lines that could not be
 * intrepreted as a different type of block. In other words, it is the
 * fallback when we fail to parse a block.
 *
 ************************************************************************/

/**
 * Check the next line for a lazy paragraph continuation.
 *
 * Check to ensure that this new line is still part of the 
 * `PARAGRAPH` that began on a previous line.
 *
 * - TODO: Add checks for blockquotes and lists.
 *
 * - parameter data: An array of byte data (input utf8 string).
 *
 * - returns: true if this is still a `PARAGRAPH`, false if this line 
 *   belongs to a different (*new*) block.
 */
static bool is_still_paragraph(uint8_t *data)
{
    return ((is_blank_line(data, CHK_SYNTX) < 0) &&
            (is_atx_header(data, CHK_SYNTX) < 0) &&
            (is_horizontal_rule(data, CHK_SYNTX) < 0) &&
            (is_opening_code_fence(data, CHK_SYNTX) < 0) &&
            (is_html_block(data, CHK_SYNTX)) < 0);
}


/**
 * Parse a paragraph block and add it to the queue.
 *
 * - parameter data: An array of byte data (utf8 string).
 *
 * - returns: The size in bytes of the block that was parsed.
 */
static ssize_t parse_paragraph(uint8_t *data)
{
    String *p = init_string(BLK_BUF);
    mdblock_t type = PARAGRAPH;
    ssize_t sh = 0;     /* Length of a possible setext header. */
    size_t  ws = 0;     /* Length of possible leading line WS. */
    
    set_current_block(PARAGRAPH);
    while (true) {
        /* Remove all leading WS on the line. */
        while (isblank(*data)) data++, ws++;
        
        /* Add characters until we reach a newline. */
        while (*data && *data != '\n') {
            if (p->length < p->allocd - NULL_CHAR) {
                *p->data++ = *data++;
                p->length++;
            }
            else realloc_string(p, p->allocd + BLK_BUF);
        }
        
        /* Is this next line the same paragraph? Setext header? */
        if (!(*data) || !is_still_paragraph(++data)) break;
        
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
    }
    
    *p->data = '\0';
    p->data -= p->length;
    add_markdown(p, type, NULL);
    
    /* <p> + [optional] setext + WS + newline [or 0 if EOF] */
    return p->length + sh + ws + (!(*data) ? 0 : NEWLINE);
}


/************************************************************************
 * ## ATX Headers
 *
 * An ATX Header is a string of bytes trapped between an opening 
 * sequence of 1-6 hashes and an optional trailing seqeunce of hashes.
 * The opening sequence must be followed by a space or tab. The closing
 * sequence must be precedded by a space and can be followed only by
 * spaces. All the above significant whitespace is stripped during
 * parsing.
 *
 ************************************************************************/

/**
 * Check the current line for an ATX header.
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter parse: If true, parse the block; if false, just check syntax.
 *
 * - returns: The size in bytes of the raw block, or -1 if not a ATX header.
 */
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
    
    /* Parse blanks until we reach a non-blank byte. */
    while (isblank(*data)) i++, data++;
    
    if (parse) return parse_atx_header(data, hashes, i);
    return i;
}


/**
 * Parse an ATX header and add it to the queue.
 *
 * This function is only called by `is_atx_header()`. In order to
 * parse a header, pass `true` to the Boolean parameter in that function.
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter hashes: Number of leading hashes (determines `mdblock_t`).
 * - parameter i: Index of `data` for the current block (return value).
 *
 * - returns: The size in bytes of the block that was parsed.
 */
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

    /* Remove any trailing spaces/hashes before the newline. */
    h->data--;
    if (*h->data == 0x20 || *h->data == '#') {
        while (*h->data == 0x20) h->data--, h->length--;
        while (*h->data == '#') h->data--, h->length--;
        
        /* Required space before trailing sequence of hashes. */
        if (*h->data == 0x20) {
            while (*h->data == 0x20) h->data--, h->length--;
        }
        /* If the space was missing, keep the trailing hashes. */
        else {
            h->data++;
            while (*h->data == '#') h->data++, h->length++;
            h->data--;
        }
    }
    h->data++;
    *h->data = '\0';
    h->data -= h->length;
    add_markdown(h, (ATX_HEADER_1 - 1) + hashes, NULL);
    return !(*data) ? i : i + NEWLINE;
}


/************************************************************************
 * ## Horizontal Rules
 *
 * A horizontal line contains a sequence of three or more matching `-`, 
 * `_`, or `*` characters, each of which may be followed many any number
 * of optional spaces.
 *
 ************************************************************************/

/**
 * Check the current line for a horizontal rule.
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter parse: If true, parse the block; if false, just check syntax.
 *
 * - returns: The size in bytes of the raw block, or -1 if not a <hr>.
 */
static ssize_t is_horizontal_rule(uint8_t *data, bool parse)
{
    size_t ws = count_indentation(data);
    size_t i  = ws;     /* Byte-index to increment and return. */
    size_t rc = 0;      /* Number of rule characters. */
    int8_t hr = -1;     /* Specific rule character used in this <hr>. */
    
    data += ws;
    hr = (*data == '*' || *data == '_' || *data == '-') ? *data : -1;
    
    /* Ensure this is not a setext header. */
    if (get_last_block() == PARAGRAPH && hr == '-') return -1;
    if (ws > 3 || hr == -1) return -1;

    /* Parse *n* number of spaces and *n* number of rule characters. */
    while (*data == 0x20 || *data == hr) {
        if (*data++ == hr) rc++;
        i++;
    }

    /* No other characters may occur inline. */
    if ((*data == '\n' || !(*data)) && rc > 2) {
        if (parse) add_markdown(NULL, HORIZONTAL_RULE, NULL);
        return !(*data) ? i : (i + NEWLINE);
    }
    return -1;
}


/************************************************************************
 * ## Setext Headers
 *
 * A setext header is a paragraph that is immediately followed by a 
 * line containing a sequence of `=` or `-` characters. Because of the 
 * ambiguity between a setext header and a horizontal rule, as each 
 * paragraph is parsed, a check is made to determine if it is a setext
 * header.
 * 
 ************************************************************************/

/**
 * Check the current line for a setext header.
 *
 * - parameter data: An array of byte data (utf8 string).
 *
 * - returns: The size in bytes of the block, or -1 if not a setext header.
 */
static ssize_t is_setext_header(uint8_t *data)
{
    size_t ws = count_indentation(data);
    size_t i  = ws;     /* Byte-index to increment and return. */
    int8_t sc = -1;     /* Setext character used in this header. */
    
    if (ws > 3) return -1;
    
    data += ws;
    sc = (*data == '-' || *data == '=') ? *data : -1;
    
    /* The last (or current) block must be a paragraph. */
    if (get_last_block() != PARAGRAPH || sc == -1) return -1;

    /* Parse *n* number of consecutive setext characters. */
    while (*data == sc) data++, i++;

    /* Parse *n* number of spaces. */
    while (*data == 0x20) data++, i++;

    /* No other characters may occur inline. */
    if (*data == '\n' || !(*data)) {
       return !(*data) ? i : (i + NEWLINE);
    }
    return -1;
}


/************************************************************************
 * ## Indented Code Blocks
 *
 * An indented code block is a series of indented lines optionally 
 * separated by blank lines. Because of the ambiguity of when an indented
 * code block actually ends, a look-a-head is performed after a blank
 * line is encountered inside an indented code block.
 *
 * For consistency, the required indentation is one tab or four spaces.
 * 
 ************************************************************************/

/**
 * Parse an indented code block and add it to the queue.
 *
 * - parameter data: An array of byte data (utf8 string).
 *
 * - returns: The size in bytes of the raw block, or -1 if not an 
 *   indented code block.
 */ 
static ssize_t parse_indented_code_block(uint8_t *data)
{
    size_t i  = 0;      /* Byte-index to increment and return. */
    size_t ws = 0;      /* White space index. */
    String *c = init_string(BLK_BUF);
    
    if ((ws = count_indentation(data)) < 4) return -1;
    
    do {
        /* Skip the indentation: one tab or four spaces. */
        if (ws > 3) {
            if (*data == '\t') data++, i++;
            else data += 4, i += 4;
        }
        else {
            /* Get first non-WS byte. */
            while (isblank(*data)) data++, i++;
            
            /* If we found a non-WS byte before the newline, break out. */
            if (*data != '\n') break;
            
            /* Otherwise add a newline and continue parsing. */
            else {
                if (c->length < c->allocd - NEWLINE - NEWLINE) {
                    *c->data++ = *data++;
                    c->length++;
                }
                else realloc_string(c, c->allocd + BLK_BUF);
                
                ws = count_indentation(data);
                continue;
            }
        }

        /* Add characters until we reach a newline. */
        while (*data && *data != '\n') {
            if (c->length < c->allocd - NEWLINE - NEWLINE) {
                *c->data++ = *data++;
                c->length++;
            }
            else realloc_string(c, c->allocd + BLK_BUF);
        }

        /* Continue parsing based on indentation -- also, be sure to
         * keep all newlines found nested in the code block. */
        if (((ws = count_indentation(++data))) > 3 || 
            is_blank_line(data, CHK_SYNTX) > 0) {
                *c->data++ = '\n';
                c->length++;
                continue;
        }
        break;
        
    } while (true);

    /* Remove any trailing newlines we added. */
    while (*(--c->data) == '\n') c->length--;
    
    *(++c->data) = '\0';
    c->data -= c->length;
    add_markdown(c, INDENTED_CODE_BLOCK, NULL);
    return i + c->length + NEWLINE;
}


/************************************************************************
 * ## Fenced Code Blocks
 *
 * A fenced code block begins with an **opening code fence** -- a series
 * of at least three consequtive, identical `\`` or `~` characters. An
 * optional info-string can also be provided after the opening fence. The
 * first 20 alpha characters after the end of the opening fence will be 
 * parsed as the blocks info-string.
 *
 * After the opening fence all subsequent lines will be parsed as the
 * content of a fenced code block until a line containing a **closing
 * code fence** is encountered. A closing fence must match the opening
 * fence in length and the character used.
 *
 ************************************************************************/

/**
 * Check the current line for an opening code fence.
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter parse: If true, parse the block; if false, just check syntax.
 *
 * - returns: The size in bytes of the line, or -1 if not a code fence.
 */
static ssize_t is_opening_code_fence(uint8_t *data, bool parse)
{
    size_t ws = count_indentation(data);
    size_t i  = ws;         /* Byte-index to increment and return. */
    int8_t fc = -1;         /* Character used in for the fence (~|`). */
    size_t fl = 0;          /* The length of this fence. */
    size_t k  = 0;          /* Index for the code fence info string. */
    CodeBlk *blk = NULL;    /* The data for this code block. */
    
    if (ws > 3) return -1;
    
    data += ws;
    fc = (*data == '`' || *data == '~') ? *data : -1;
    if (fc == -1) return -1;

    /* Count the number of fence characters. */
    while (*data == fc) i++, fl++, data++;
    if (fl < 3) return -1;
    
    /* Save the code block data if we're parsing, return true (positive
     * integer) to the caller if we were sent here to just check syntax. */
    if (parse) {
        blk = init_code_blk();
        blk->ws = ws;
        blk->fl = fl;
        blk->fc = fc;
    }
    else return i;
    
    /* Skip an unlimited number of whitespace. */
    while (*data == 0x20 || *data == '\t') i++, data++;
    
    /* Enter the fenced code block if there's no info string. */
    if (*data == '\n') {
        if (parse) {
            return parse_fenced_code_block(++data, blk, i + NEWLINE);
        }
        return !(*data) ? i : (i + NEWLINE);
    }
    
    /* Parse the info string. */
    for (k = 0; (k < INFO_STR_MAX && isalpha(*data)); k++) {
        blk->lang[k] = *data++;
        i++;
    }
    blk->lang[k] = '\0';
    
    /* Find the newline. */
    while (*data && *data != '\n') data++, i++;
    
    if (parse) {
        return parse_fenced_code_block(++data, blk, i + NEWLINE);
    }
    return !(*data) ? i : (i + NEWLINE);
}


/**
 * Check the current line for a closing code fence.
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter blk: Data about the opening fence for this block.
 *
 * - returns: The size in bytes of the line, or -1 if not a code fence.
 */
static ssize_t is_closing_code_fence(uint8_t *data, CodeBlk *blk)
{
    size_t ws = count_indentation(data);
    size_t i  = ws;         /* Byte-index to increment and return. */
    int8_t fc = -1;         /* Character used for closing fence (~|`). */
    size_t fl = 0;          /* The length of the closing fence. */
    
    if (ws > 3) return -1;
    
    data += ws;
    fc = (*data == '`' || *data == '~') ? *data : -1;
    if (fc == -1 || fc != blk->fc) return -1;

    /* Count the number of fence characters. */
    while (*data == fc) i++, fl++, data++;
    if (fl < 3 || fl < blk->fl) return -1;
    
    /* Skip an unlimited number of whitespace. */
    while (*data == 0x20 || *data == '\t') i++, data++;
    
    /* If any non-newline characters, this can't be a closing fence. */
    if (!(*data)) return i;
    if (*data == '\n') return i + NEWLINE;
    
    return -1;
}


/**
 * Parse a fenced code block and add it to the queue.
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter blk: Data about the opening fence for this block.
 * - parameter i: Index of `data` for the current block (return value).
 *
 * - returns: The size in bytes of the block.
 */
static size_t parse_fenced_code_block(uint8_t *data, CodeBlk *blk, size_t i)
{
    String *cb  = init_string(BLK_BUF);
    ssize_t cfl = 0;    /* Closing-fence line length (in bytes). */
    
    /* Parse every line as part of this code block 
     * until we find the closing fence. */
    while (true) {
        
        /* Check this line for a code fence. */
        if ((cfl = is_closing_code_fence(data, blk)) > 0) break;
        cfl = 0;
        
        /* Advance past the WS on the opening code fence. */
        size_t linews = 0;
        while (blk->ws > 0 && *data == 0x20 && linews < blk->ws) {
            data++, i++;
            linews++;
        }
        
        /* Add characters until we reach a newline. */
        while (*data && *data != '\n') {
            if (cb->length < cb->allocd - NEWLINE - NULL_CHAR) {
                *cb->data++ = *data++;
                cb->length++;
                i++;
            }
            else realloc_string(cb, cb->allocd + BLK_BUF);
        }
        
        /* Add the newline. */
        *cb->data++ = '\n';
        cb->length++;
        
        if (!(*data)) break;
        i++, data++;
    }
    
    *cb->data = '\0';
    cb->data -= cb->length;
    add_markdown(cb, FENCED_CODE_BLOCK, blk);
    return i + cfl;
}


/************************************************************************
 * ## HTML Blocks
 *
 * Because there are 7 different kinds of HTML blocks, all traffic is
 * directed through `is_html_block()` which will examine the syntax of 
 * the current line and determine which type -- if any -- can be 
 * represented. It will then parse the line by calling the appropriate
 * parsing function if the `parse` parameter is true.
 *
 * The seven types of HTML blocks:
 *
 * 1. **Literal content**: Line begins with `<script`, `<pre`, or `<style`. 
 *
 *    This block is closed when an end tag -- `</script>`, `</pre>`, or 
 *    `<style>` --  is encountered. As a result, these blocks can contain 
 *    blank lines. If there is no end tag, the rest of the document will 
 *    be part of this block. The end tag can also occur on the same line 
 *    as the opening tag. Anything after the end tag on the line it is
 *    found is considered part of this block.
 *
 * 2. **HTML Comment**: line begins with `<!--`.
 *
 *    This block is closed when the end tag -- `-->` -- is encountered.
 *    It follows the same rules regarding the end tag as type #1.
 *
 * 3. **PHP instructions**: line begins with `<?`.
 *
 *    This block is closed when the end tag -- `?>` -- is encountered.
 *    It follows the same rules regarding the end tag as type #1.
 *
 * 4. **HTML declaration**: line begins with `<!` followed by an uppercase
 *    ASCII character.
 *
 *    This block is closed when the end tag -- `>` -- is encountered.
 *    It follows the same rules regarding the end tag as type #1.
 *
 * 5. **CDATA instructions**: line begins with `<![CDATA[`.
 *
 *    This block is closed when the end tag -- `]]>` -- is encountered.
 *    It follows the same rules regarding the end tag as type #1.
 *
 * 6. **HTML5 Element**: line begins with `<` or `</` followed by one of
 *    the standard element names.
 *
 *    This block is closed when a blank line is encountered. Therefore,
 *    there can be no nested blank lines inside this block. No checks 
 *    are made to ensure the opening tag is closed or even that it is
 *    complete -- only the bracket, optional forward-slash, and element
 *    name are required.
 *
 * 7. **Custom Element**: line begins with `<` or `</` followed by any tag
 *    name that is not `script`, `style`, or `pre` followed by a `>` and
 *    any amount of whitespace before the newline.
 *
 *    This block is closed when a blank line is encountered. This block
 *    cannot interrupt a paragraph. A distinction is made from **HTML5
 *    Elements** in order to prevent the parser from closing a paragraph 
 *    prematurely when a tag is placed at the beginning of the line of a 
 *    lazy-continuation.
 *
 ************************************************************************/


/**
 * Match an input string to a valid HTML element name.
 *
 * - parameter e: The input string.
 * - parameter len: The number of characters in `e`.
 *
 * - returns: true if a match is found, false otherwise.
 */
static bool match_html_element(uint8_t *e, size_t len)
{
    if (len > 7) len = 7;
    
    /* Valid element names -- separated by string length. */
    static char *elements[8][17] = {
        { NULL },
        { 
            "p", NULL 
        },
        { 
            "dd", "dl", "dt", "h1", "h2", "h3", "h4", "h5", "h6", "hr", 
            "li", "ol", "td", "th", "tr", "ul", NULL 
        },
        { 
            "col", "dir", "div", "nav", NULL 
        },
        { 
            "base", "body", "form", "head", "html", "link", "main", 
            "menu", "meta", NULL 
        },
        { 
            "aside", "frame", "param", "table", "tbody", "tfoot",
            "thead", "title", "track", NULL
        },
        { 
            "center", "dialog", "figure", "footer", "header", "iframe",
            "legend", "option", "source", NULL
        },
        { 
            "address", "article", "basefont", "blockquote", "caption", 
            "colgroup", "details", "fieldset", "figcaption", "frameset", 
            "menuitem", "noframes", "optgroup", "section", "summary", NULL
        }
    };

    if (len == 0) return false;

    for (size_t i = 0; i < 17; i++) {
        if (!elements[len][i]) break;
        if (strncmp((char *)e, elements[len][i], TAG_LEN) == 0) {
            return true;
        }
    }
    return false;
}


/**
 * Search the current line for the substring `endtag`.
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter endtag: The string of characters that ends the block.
 *
 * - returns: true if the line contains `endtag`, false otherwise.
 */
static bool line_contains_endtag(char *data, const char *endtag)
{
    size_t index = 0;       /* Index of next newline. */
    size_t tagi  = 0;       /* Index of the next end tag. */
    char *tag    = NULL;    /* Pointer to next end tag. */
    char *newl   = NULL;    /* Pointer to next newline. */
    
    /* Get index of next newline. */
    if (!(newl = strchr(data, '\n'))) {
        return false;
    }
    index = newl - data;
    
    /* Find the index of the endtag. */
    if (!(tag = strstr(data, endtag))) {
        return false;
    }
    tagi = tag - data;
    
    return tagi < index;
}


/**
 * Parse all input as an HTML block until a blank line is encountered.
 *
 * Add the HTML block to the queue after parsing. The blank line that 
 * ends this HTML block will not be parsed, we just check for it's
 * existence, then break out of parsing the HTML block. The next
 * iteration of the main parsing loop will add that blank line to the 
 * markdown queue.
 *
 * - parameter data: An array of byte data (utf8 string).
 *
 * - returns: The size in bytes of the block.
 */
static ssize_t parse_html_until_blankline(uint8_t *data)
{
    String *hb  = init_string(BLK_BUF);
    size_t i    = 0;    /* Byte-index to increment and return. */
    ssize_t bl  = 0;    /* Index returned from is_blank_line(). */
    
    while (true) {
        
        /* Add characters until we reach a newline. */
        while (*data && *data != '\n') {
            if (hb->length < hb->allocd - NEWLINE - NULL_CHAR) {
                *hb->data++ = *data++;
                hb->length++;
                i++;
            }
            else realloc_string(hb, hb->allocd + BLK_BUF);
        }
        
        /* Check the next line for a blank line. */
        if ((bl = is_blank_line(++data, CHK_SYNTX)) >= 0) break;
        else {
            *hb->data++ = '\n';
            hb->length++;
            i++;
            bl = 0;
        }
    }
    
    *hb->data = '\0';
    hb->data -= hb->length;
    add_markdown(hb, HTML_BLOCK, NULL);
    return i + bl;
}


/**
 * Parse all input as an HTML block until a proper end tag is found.
 *
 * This function parses types 1 - 5 of HTML Blocks (literal content,
 * HTML content, PHP instructions, HTML declarations, CDATA 
 * instructions).
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter endtag: The string of characters that ends the block.
 *
 * - returns: The size in bytes of the block. 
 */
static ssize_t parse_html_block(uint8_t *data, const char *endtag)
{
    String *hb = init_string(BLK_BUF);
    bool lastline = false;      /* Set to true when block should end. */
    
    while (true) {
        
        /* Check if the current line contains the end tag. */
        if (line_contains_endtag((char *)data, endtag)) {
            lastline = true;
        }
        
        /* Add characters until we reach a newline. */
        while (*data && *data != '\n') {
            if (hb->length < hb->allocd - NEWLINE - NULL_CHAR) {
                *hb->data++ = *data++;
                hb->length++;
            }
            else realloc_string(hb, hb->allocd + BLK_BUF);
        }
        
        /* Break if that was our last line or EOF. */
        if (!(*data) || lastline) break;
        
        /* Add newline if we're still parsing. */
        *hb->data++ = *data++;
        hb->length++;
    }
    
    *hb->data = '\0';
    hb->data -= hb->length;
    add_markdown(hb, HTML_BLOCK, NULL);
    return hb->length + NEWLINE;
}


/**
 * Check the current line for an HTML block.
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter parse: If true, parse the block; if false, just check syntax.
 *
 * - returns: The size in bytes of the line, or -1 if not an HTML block.
 */
static ssize_t is_html_block(uint8_t *data, bool parse)
{
    size_t ws = count_indentation(data);
    size_t i  = ws;         /* Byte-index to increment and return. */
    size_t k  = 0;          /* Index for the tag buffer. */
    uint8_t tag[TAG_LEN];   /* Buffer to hold the tag while parsing. */
    bool literal = true;    /* Should we parse for type 1: literal content. */
    
    if (ws > 3) return -1;
    data += ws;
    
    /* All html tags must be opened. */
    if (*data++ != '<') return -1;
    else i++;
    
    /* HTML comments, HTML declarations, and CDATA instructions. */
    if (*data == '!') {
        data++, i++;
        
        /* 2nd type: HTML comment. */
        if (*data == '-') {
            data++, i++;
            if (*data == '-') {
                return parse ? parse_html_block(data - i, "-->") : i;
            }
            else return -1;
        }
        
        /* 4th type: HTML declaration. */
        else if (isupper(*data)) {
            return parse ? parse_html_block(data - i, ">") : i;
        }
        
        /* 5th type: CDATA instructions. */
        else if (*data == '[') {
            data++, i++;
            if (*data++ == 'C' &&
                *data++ == 'D' &&
                *data++ == 'A' &&
                *data++ == 'T' &&
                *data++ == 'A' &&
                *data   == '[') {
                    i += 5;
                    return parse ? parse_html_block(data - i, "]]>") : i;
            }
            else return -1;
        }
        else return -1;
    }
    
    /* 3rd type: PHP instructions. */
    if (*data == '?') {
        return parse ? parse_html_block(data - i, "?>") : i;
    }
    
    /* Check for optional forward-slash -- rules out literal blocks. */
    if (*data == '/') {
        data++, i++;
        literal = false;
    }
    
    /* Extract the tag name -- exit if there's no tag. */
    for (k = 0; k < TAG_LEN - 1 && isalpha(*data); k++, i++) {
        tag[k] = tolower(*data++);
    }
    tag[k] = '\0';
    if (k == 0) return -1;
    
    /* 1st type: Literal content. */
    if (literal) {
        if (strncmp((char *)tag, "script", TAG_LEN) == 0) {
            return parse ? parse_html_block(data - i, "</script>") : i;
        }
        else if (strncmp((char *)tag, "style", TAG_LEN) == 0) {
            return parse ? parse_html_block(data - i, "</style>") : i;
        }
        else if (strncmp((char *)tag, "pre", TAG_LEN) == 0) {
            return parse ? parse_html_block(data - i, "</pre>") : i;
        }
    }
    
    /* 6th type: HTML5 element. */
    if (match_html_element(tag, k)) {
        return parse ? parse_html_until_blankline(data - i) : i;
    }
    
    /* 7th type: Custom element -- cannot interrupt a paragraph. */
    if (get_last_block() == PARAGRAPH) return -1;
    
    /* Only the opening bracket is allowed on the first line. */
    while (*data && *data != '>' && *data != '\n') data++, i++;
    if (*data == '\n') return -1;
    else data++, i++;
    
    /* Find the newline, otherwise this can't be custom element. */
    while (*data == 0x20) data++, i++;
    if (*data && *data != '\n') return -1;
    
    return parse ? parse_html_until_blankline(data - i) : i;
}


/************************************************************************
 * ## Link Reference Definitions
 *
 * Definitions of **link references** take the following form:
 *
 * 1. 0-3 spaces of indentation.
 * 2. *Link label* followed by a colon: `[link label]:`
 * 3. Unlimited amount of WS -- including a newline.
 * 4. *Link destination*: 999 consecutive non-control, non-space ASCII
 * 5. Unlimited amount of WS -- including a newline.
 * 6. Optional *Link title*: sequence of quoted characters
 *
 * Links are stored in a binary search tree internal to the 
 * implementation in `links.c`. They are also inserted into the Markdown
 * queue for testing purposes.
 *
 ************************************************************************/

/**
 * Check the current line for a link reference definition.
 *
 * - TODO: Add this node to the links BST if parsing was successful.
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter parse: If true, parse the block; if false, just check syntax.
 *
 * - returns: The size in bytes of the line, or -1 if not a link definition.
 */
static ssize_t is_link_definition(uint8_t *data, bool parse)
{
    size_t ws = count_indentation(data);
    size_t i  = ws;     /* Byte-index to increment and return. */
    size_t k  = 0;      /* Index for the link label, dest, and title. */
    LinkRef *lr = init_link_ref();
    
    if (ws > 3) return -1;
    data += ws;
    
    /* Opening bracket for the link label. */
    if (*data != '[') return -1;
    data++, i++;
    
    /* Add characters until we reach the closing bracket. */
    for (k = 0; k < 1000 && *data && *data != ']'; k++, i++) {
        lr->label[k] = *data++;
    }
    lr->label[k] = '\0';
    
    /* Ensure we found the closing bracket and colon. */
    if (*data != ']') return -1;
    data++, i++;
    if (*data != ':') return -1;
    data++, i++;
    
    /* Skip an unlimited amount of spaces, tabs, and an optional newline. */
    while (*data == 0x20 || *data == '\t') data++, i++;
    if (*data == '\n') data++, i++;
    while (*data == 0x20 || *data == '\t') data++, i++;
    
    /* Link reference definitions must provide a destination. */
    if (!(*data) || *data == '\n') return -1;
    
    /* Parse destination until a space or control character. */
    if (*data == '<') data++, i++;
    for (k = 0; k < 1000 && isgraph(*data); k++, i++) {
        lr->dest[k] = *data++;
    }
    if (*(data - 1) == '>') lr->dest[--k] = '\0';
    else lr->dest[k] = '\0';
    
    /* Skip an unlimited amount of spaces, tabs, and an optional newline. */
    while (*data == 0x20 || *data == '\t') data++, i++;
    if (*data == '\n') data++, i++;
    while (*data == 0x20 || *data == '\t') data++, i++;
    
    /* Check for opening to a link title. */
    if (*data == '\'' || *data == '\"') {
        uint8_t titleEnd = *data++;
        
        /* Add characters until we reach the end of the title. */
        for (k = 0; k < 1000 && *data && *data != titleEnd; k++, i++) {
            lr->title[k] = *data++;
        }
        lr->title[k] = '\0';
        if (*data == titleEnd) data++, i++;
        
        /* Skip an unlimited amount of spaces and tabs. */
        while (*data == 0x20 || *data == '\t') data++, i++;
    }
    
    if (parse) add_markdown(NULL, LINK_REFERENCE_DEF, lr);
    return i + NEWLINE;
}


/************************************************************************
 * ## Blockquotes
 *
 * There are two different types of blockquotes, either of which can
 * interrupt a paragraph:
 *
 * 1. **Basic Case**: a sequence of sequential lines that all begin with
 *    0-3 spaces of WS followed by a `>`.
 *
 * 2. **Lazy Case**: If a blockquote begins with the *base case* and a
 *    paragraph is entered, all subseqent lines of that paragraph can
 *    have lazy-continuation **without** the prepending `>`.
 *
 ************************************************************************/

/**
 * Parse all subsequent lines with a blockquote marker.
 *
 * This function should only be called by `is_blockquote()`. It works by
 * collecting the complete contents of the blockquote into a `String` 
 * node and calling `parse_block()` on that content.
 *
 * - parameter data: An array of byte data (utf8 string).
 *
 * - returns: The size in bytes of the block.
 */
static size_t parse_blockquote(uint8_t *data)
{
    size_t ws  = 0;     /* Whitespace for current line. */
    size_t i   = 0;     /* Byte-index to increment and return. */
    bool first = true;  /* Flag to determine if first line of content. */
    
    /* String to hold contents of blockquote. */
    String *bq = init_string(BLK_BUF);
    
    /* Parse blockquote line-by-line. */
    while (true) {
        
        /* Skip indentation. */
        ws = count_indentation(data);
        if (ws > 3 || *data == '\t') break;
        data += ws, i += ws;
        
        /* Required prepending blockquote character. */
        if (*data != '>') {
            data -= ws, i -= ws;

            /* Check for lazy case (type 2). */
            if (is_still_paragraph(data)) {

                /* Get the last line we added. */
                uint8_t *lastline = NULL;
                bq->data -= bq->length;

                for (size_t k = 0; k < bq->length; k++) {
                    if (bq->data[k] == '\n') lastline = &bq->data[++k];
                }
                
                /* If theres no newline but content, we only have one line. */
                if (!lastline && bq->length > 0) lastline = bq->data;
                else if (!lastline) break;
                bq->data += bq->length;

                /* If last line was paragraph, this is continue on. */
                if (!is_still_paragraph(lastline)) break;
            }
            else break;
        }
        else data++, i++;
        
        /* Skip one space -- if it's there. */
        if (*data == 0x20) data++, i++;
        
        /* Add newline if we're still parsing. */
        if (!first) {
            *bq->data++ = '\n';
            bq->length++;
        }
        
        /* Add characters until we reach a newline. */
        while (*data && *data != '\n') {
            if (bq->length < bq->allocd - NEWLINE - NULL_CHAR) {
                *bq->data++ = *data++;
                bq->length++;
            }
            else realloc_string(bq, bq->allocd + BLK_BUF);
        }
        if (!(*data)) break;
        
        data++;
        first = false;
    }
    
    *bq->data = '\0';
    bq->data -= bq->length;
    
    add_markdown(NULL, BLOCKQUOTE_START, NULL);
    block_parser(bq);
    add_markdown(NULL, BLOCKQUOTE_END, NULL);
    
    return bq->length + i + NEWLINE;
}

/**
 * Check the current line for the beginning of a blockquote.
 *
 * - parameter data: An array of byte data (utf8 string).
 * - parameter parse: If true, parse the block; if false, just check syntax.
 *
 * - returns: The size in bytes of the line, or -1 if not a link definition.
 */
static ssize_t is_blockquote(uint8_t *data, bool parse)
{
    size_t ws = count_indentation(data);
    size_t i  = ws;     /* Byte-index to increment and return. */
    
    if (ws > 3 || *data == '\t') return -1;
    data += ws;
    
    /* Required prepending blockquote character. */
    if (*data != '>') return -1;
    
    return parse ? parse_blockquote(data - i) : ++i;
}
