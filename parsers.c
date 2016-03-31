/* parsers.c
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 03/04/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file contains the implementations of functions and structures that 
 * parse lines read from the input file to determine their mdblock_t and
 * strip the string of its metacharacters and WS.
 * ======================================================================= */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parsers.h"
#include "markdown.h"
#include "utilities.h"
#include "errors.h"

/* Static helper functions for use only by the functions in this file.
 * ======================================================================= */
static ParseInfo *alloc_parse_info(mdblock_t type, size_t start, size_t stop, insert_t insert);
static void reset_parsing_variables(ParseInfo *parse);
static ParseInfo *check_last_block_type(char *line);


/* Global variables known only to the parsing functions in this file.
 * ======================================================================= */
mdblock_t lastBlock = UNKNOWN; // mdblock_t of most recently parsed block.
char lastFencedBlockMark;      // The mark used for most recent fenced code block.
char *linkLabelStr  = NULL;    // A link label, used when parsing LinkRef's.
char *linkurlStr    = NULL;    // A link URL, used when parsing LinkRef's.
char *linkTitleStr  = NULL;    // A link title, used when parsing LinkRef's.


/* parse_block_type(char *, mdblock_t)
 * =======================================================================
 * This is the grand central station of block-level parsing. Given a line
 * from input, determine its mdblock_t and parse it to remove all WS and
 * metacharacters.
 *
 * Returns a TempMarkdownBlock which contains the mdblock_t, parsed 
 * string, and insert_t of the input string. The TempMarkdownBlock now 
 * contains all the necessary information to be added to the stack.
 * ======================================================================= */
TempMarkdownBlock *parse_block_type(char *line)
{
    ParseInfo *parse         = check_last_block_type(line);
    TempMarkdownBlock *block = NULL;
    char *linePtr            = line;
    char c, *newstr;
    
    while ((c = *linePtr++))
    {
        if (parse != NULL) break;
        switch (c)
        {
            case ' ': parse = parse_indented_code_block(line); break;
            case '#': parse = parse_atx_heading(line); break;
            case '*': parse = parse_horizontal_rule(line, '*'); break;
            case '_': parse = parse_horizontal_rule(line, '_'); break;
            case '-': parse = parse_horizontal_rule(line, '-'); break;
            case '=': parse = parse_setext_heading(line, '='); break;
            case '`': parse = parse_code_fence(line, '`'); break;
            case '~': parse = parse_code_fence(line, '~'); break;
            case '<': parse = parse_html_block(line); break;
            case '[': parse = parse_link_definition(line); break;
            default: break;
        }
    }
    if (parse == NULL) parse = parse_paragraph(line);
    
    // PLACEHOLDERs are not enqueued, therefore string == NULL
    if (parse->insert == PLACEHOLDER) 
    {
        block = alloc_temp_block(NULL, BLANK_LINE, PLACEHOLDER, NULL);
    }
    else if (parse->insert == INSERT_LINK)
    {
        LinkRef *alink = alloc_link_ref(linkLabelStr, linkurlStr, linkTitleStr);
        block = alloc_temp_block(NULL, LINK_REF_DEFINITION, INSERT_LINK, alink);
    }
    else
    {
        newstr = create_substr(line, parse->start, parse->stop);
        if (newstr) block = alloc_temp_block(newstr, parse->type, parse->insert, NULL);
        else        block = alloc_temp_block(line, parse->type, parse->insert, NULL);
    }
    
    reset_parsing_variables(parse);
    return block;
}


/* parse_atx_heading(char *)
 * =======================================================================
 * ATX Headings are a string between an opening sequence of indentation 
 * and pound-signs and a closing sequence of WS and pound-signs. All 
 * leading and trailing WS and pound-signs are marked for removal.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return a PARAGRAPH or an INDENTED_CODE_BLOCK.
 * ======================================================================= */
ParseInfo *parse_atx_heading(char *s)
{
    char c;
    size_t level        = 0; // Level of the heading.
    size_t indentation  = 0; // Amount of WS before the first `#`.
    size_t preHeadingWS = 0; // Amount of WS after the last `#`.
    char *sPtr          = s; // Point to the first char in s.
    size_t strlength    = strlen(s);
    char *sPtr2         = &s[strlength - 1]; // Point to last char in s.
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation > 3) return parse_indented_code_block(s);
    
    while (c == '#') { level++; c = *sPtr++; }
    if (level > 6) return parse_paragraph(s);
    
    // At least one space required between last `#` and first non-WS character.
    while (c == ' ') { preHeadingWS++; c = *sPtr++; }
    if (preHeadingWS < 1) return parse_paragraph(s);
    
    // Check for empty ATX Headings
    if (c == '\0' || c == '#') strlength = indentation + level + preHeadingWS;
    else
    {
        // Remove all trailing WS and `#` characters from heading.
        while ((c = *sPtr2--) == ' ') strlength--;
        while (c == '#') { strlength--; c = *sPtr2--; }
        while (c == ' ') { strlength--; c = *sPtr2--; }
    }
    
    switch (level)
    {
        case 1:  return alloc_parse_info(ATX_HEADING_1, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        case 2:  return alloc_parse_info(ATX_HEADING_2, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        case 3:  return alloc_parse_info(ATX_HEADING_3, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        case 4:  return alloc_parse_info(ATX_HEADING_4, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        case 5:  return alloc_parse_info(ATX_HEADING_5, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        case 6:  return alloc_parse_info(ATX_HEADING_6, (indentation + level + preHeadingWS), strlength, INSERT_NODE);
        default: return parse_paragraph(s);
    }
}


/* parse_paragraph(char *)
 * =======================================================================
 * Paragraphs are a sequence of lines containing at least one non-WS
 * character. They are used as the "fall-back mdblock_t"; in other words,
 * they are the last resort when the mdblock_t cannot be determined. 
 * Because of this, there is not a lot of parsing to be done, it is likely
 * that all other options have been exhausted by the time this function is
 * called. All we do is remove the leading WS.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return an INDENTED_CODE_BLOCK.
 * ======================================================================= */
ParseInfo *parse_paragraph(char *s)
{
    char c;
    char *sPtr = s;          // Point to the first character in s.
    size_t indentation  = 0; // Amount of WS before the first non-WS char.
    size_t maxLeadingWS = 3; // Max number of valid leading WS chars.
    
    // If we are appending this line, the indentation can be unlimited.
    if (lastBlock == PARAGRAPH) maxLeadingWS = 100;
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation > maxLeadingWS) return parse_indented_code_block(s);
    
    if (lastBlock == PARAGRAPH)
    {
        return alloc_parse_info(PARAGRAPH, indentation, strlen(s), APPEND_STRING);
    }
    else return alloc_parse_info(PARAGRAPH, indentation, strlen(s), INSERT_NODE);
}


/* parse_indented_code_block(char *)
 * =======================================================================
 * Indented code blocks are composed of lines indented with at least four
 * leading WS characters. Any less than 4, and the block cannot be an 
 * indented code block. Lines with only WS are indented code blocks if and
 * only if they have at least 4 characters of WS indentation, otherwise 
 * they are BLANK_LINEs.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return a PARAGRAPH or NULL.
 * ======================================================================= */
ParseInfo *parse_indented_code_block(char *s)
{
    // INDENTED_CODE_BLOCKs cannot interrupt PARAGRAPHs.
    if (lastBlock == PARAGRAPH) return NULL;
    
    char c;
    char *sPtr = s;         // Point to the first char in s.
    size_t indentation = 0; // Amount of WS before the fist non-WS char.
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation < 4 && c == '\0') return alloc_parse_info(BLANK_LINE, 0, 1, PLACEHOLDER);
    else if (indentation < 4) return NULL;
    
    if (lastBlock == INDENTED_CODE_BLOCK)
    {
        return alloc_parse_info(INDENTED_CODE_BLOCK, 4, strlen(s), APPEND_STRING);
    }
    else return alloc_parse_info(INDENTED_CODE_BLOCK, 4, strlen(s), INSERT_NODE);
}


/* parse_horizontal_rule(char *, char)
 * =======================================================================
 * Horizontal rules are lines consisting of three or more matching `-`, 
 * `_`, or `*` characters, each followed by an unlimited number of WS. 
 * Lines containing less than 3 of these marks are parsed as paragraphs.
 * These lines cannot contain any other characters than the chosen mark
 * and WS, if any other character is encountered, the line is parsed as a
 * paragraph.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return a PARAGRAPH, INDENTED_CODE_BLOCK, or SETEXT_HEADING_2.
 * ======================================================================= */
ParseInfo *parse_horizontal_rule(char *s, char mark)
{
    // If any ambiguity choose SETEXT_HEADING_2
    if (lastBlock == PARAGRAPH && mark == '-') return parse_setext_heading(s, mark);
    
    char c;
    char *sPtr = s;         // Point to the first char in s.
    size_t indentation = 0; // Amount of WS before the first non-WS char.
    size_t numberMarks = 0; // Number of *marked* characters encountered.
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation > 3) return parse_indented_code_block(s);
    
    // Unlimited number of marks separated by unlimited spaces
    while (c == mark || c == ' ') 
    {
        if (c == mark) numberMarks++;
        c = *sPtr++;
    }
    
    // If we encountered any character other than mark or WS, return PARAGRAPH
    if (c != '\0') return parse_paragraph(s);
    if (numberMarks < 3) return parse_paragraph(s);
    
    return alloc_parse_info(HORIZONTAL_RULE, 0, 1, INSERT_NODE);
}


/* parse_setext_heading(char *, char)
 * =======================================================================
 * Setext headings are a line consisting of a single character, similar to
 * horizontal rules, in the block immediately following a paragraph. They
 * serve the upgrade the previous block from a paragraph to a header of 
 * level 1 or 2.
 *
 * NOTE: When the mark is '-', parse_block_type() will call the horizontal
 *       rule parser first, and it will call this parser.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return NULL, INDENTED_CODE_BLOCK, or PARAGRAPH.
 * ======================================================================= */
ParseInfo *parse_setext_heading(char *s, char mark)
{
    // Last block must be a PARAGRAPH to be parsed as a SETEXT_HEADING_x
    if (lastBlock != PARAGRAPH) return NULL;
    
    char c;
    char *sPtr = s;         // Point to the first char in s.
    size_t indentation = 0; // Amount of WS before the first non-WS char.
    size_t numberMarks = 0; // Number of *marked* characters encountered.
    size_t trailingWS  = 0; // Amount of WS after the last *mark*.
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation > 3) return parse_indented_code_block(s);
    
    while (c == mark) { numberMarks++; c = *sPtr++; }
    if (numberMarks < 1) return NULL;
    
    while (c == ' ') { trailingWS++; c = *sPtr++; }
    
    if (c != '\0') return parse_paragraph(s);
    
    switch (mark)
    {
        case '=': return alloc_parse_info(SETEXT_HEADING_1, 0, 1, UPDATE_TYPE);
        case '-': return alloc_parse_info(SETEXT_HEADING_2, 0, 1, UPDATE_TYPE);
        default:  return parse_paragraph(s);
    }
}


/* parse_code_fence(char *, char)
 * =======================================================================
 * The opening (FENCED_CODE_BLOCK_START) and closing 
 * (FENCED_CODE_BLOCK_STOP) fences are the only thing parsed in this 
 * function. But because we can **guarantee** that this will be the first
 * parsing function called if the last container block was either a
 * FENCED_CODE_BLOCK or a FENCED_CODE_BLOCK_START, we return 
 * FENCED_CODE_BLOCK if we can determine that we are **in** a fenced code
 * block container and the current line is **not** the closing fence.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return INDENTED_CODE_BLOCK, FENCED_CODE_BLOCK, and NULL.
 * ======================================================================= */
ParseInfo *parse_code_fence(char *s, char mark)
{
    char c;
    char *sPtr = s;         // Point to the first char in s.
    size_t indentation = 0; // Amount of WS before the first non-WS char.
    size_t numberMarks = 0; // Number of *marked* characters encountered.
    size_t infoStrWS   = 0; // Number WS after marks and before info string.
    size_t infoString  = 0; // Amount of WS after the last *mark*.
    size_t start       = 0; // Sum of indentation, numberMarks, infoStrWS
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation > 3 && (lastBlock != FENCED_CODE_BLOCK && lastBlock != FENCED_CODE_BLOCK_START))
    {
        return parse_indented_code_block(s);
    }
    
    while (c == mark) { numberMarks++; c = *sPtr++; }
    
    // If no marks, then this is either a FENCED_CODE_BLOCK or we need more parsing.
    if (numberMarks == 0 && lastBlock == FENCED_CODE_BLOCK_START)
    {
        return alloc_parse_info(FENCED_CODE_BLOCK, 0, strlen(s), INSERT_NODE);
    }
    else if (numberMarks == 0 && lastBlock == FENCED_CODE_BLOCK)
    {
        return alloc_parse_info(FENCED_CODE_BLOCK, 0, strlen(s), APPEND_STRING);
    }
    else if (numberMarks < 3) { printf("NULL;\n"); return NULL;}
    
    // Optional number of spaces before info string.
    while (c == ' ') { infoStrWS++; c = *sPtr++; }
    
    // Count the number of characters in the info string.
    while (isalpha(c)) { infoString++; c = *sPtr++; }
    
    start = indentation + numberMarks + infoStrWS;
    if (infoString == 0 && mark != lastFencedBlockMark) return NULL;
    else if (infoString == 0)
    {
        lastFencedBlockMark = ' ';
        return alloc_parse_info(FENCED_CODE_BLOCK_STOP, 0, 1, INSERT_NODE);
    }
    else 
    {
        lastFencedBlockMark = mark;
        return alloc_parse_info(FENCED_CODE_BLOCK_START, start, start + infoString, INSERT_NODE);
    }
}


/* parse_html_block(char *,)
 * =======================================================================
 * HTML blocks start with a line where the first character is a left-angle
 * bracket, `<`.  Every consecutive line will be appended to this 
 * HTML_BLOCK mdblock_t until we encounter a newline, in which the 
 * container will end.
 *
 * Returns a structure of information required to create the output string.
 * NOTE: Can return INDENTED_CODE_BLOCK and BLANK_LINE
 * ======================================================================= */
ParseInfo *parse_html_block(char *s)
{
    char c;
    char *sPtr = s;          // Point to the first char in s.
    size_t indentation  = 0; // Amount of WS before the first non-WS char.
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation > 3 && lastBlock != HTML_BLOCK) return parse_indented_code_block(s);
    
    if (c != '\0')
    {
        if (lastBlock == HTML_BLOCK)
        {
            return alloc_parse_info(HTML_BLOCK, 0, strlen(s), APPEND_STRING);
        }
        else return alloc_parse_info(HTML_BLOCK, 0, strlen(s), INSERT_NODE);
    }
    else
    {
        return alloc_parse_info(BLANK_LINE, 0, 0, PLACEHOLDER);
    }
}


/* parse_link_definition(char *)
 * =======================================================================
 * Link definitions start with a link label surrounded in brackets, 
 * followed by a mandatory colon, then any optional amount of WS, then a
 * mandatory link URL, followed by an optional amount of WS, and ending 
 * with an optional link title, surrounded in double or single quotes.
 *
 * NOTE: Currently, all 3 (label, url, title) portions of a link reference
 *       must be located on the same line in the input file.
 *
 * Returns a structure of information required to create the LinkRef node.
 * NOTE: Can return INDENTED_CODE_BLOCK, NULL, or PARAGRAPH.
 * ======================================================================= */
ParseInfo *parse_link_definition(char *s)
{
    char c;
    char *sPtr = s;          // Point to the first char in s.
    size_t indentation  = 0; // Amount of WS before the first non-WS char.
    size_t openBracket  = 0; // Required left bracket to start link label.
    size_t linkLabel    = 0; // Number of characters in the link label.
    size_t closeBracket = 0; // Required right bracket to end link label.
    size_t colon        = 0; // Required colon to close link label.
    size_t preDestWS    = 0; // Optional WS between colon and URL.
    size_t destURL      = 0; // URL of destination.
    size_t postDestWS   = 0; // Optional WS between URL and link title.
    size_t openQuote    = 0; // Opening quote for link title.
    size_t linkTitle    = 0; // Optional link title.
    size_t closeQuote   = 0; // Closing quote for link title.
    
    while ((c = *sPtr++) == ' ') indentation++;
    if (indentation > 3) return parse_indented_code_block(s);
    
    if (c == '[') { openBracket++; c = *sPtr++;}
    else return NULL;
    
    while (c != ']') { linkLabel++; c = *sPtr++; }
    if (linkLabel == 0) return parse_paragraph(s);
    
    if (c == ']') closeBracket++;
    else return parse_paragraph(s);
    
    if ((c = *sPtr++) == ':') colon++;
    else return parse_paragraph(s);
    
    while ((c = *sPtr++) == ' ') preDestWS++;
    
    while (isgraph(c)) { destURL++; c = *sPtr++; }
    if (destURL == 0) return parse_paragraph(s);
    
    while (c == ' ') { postDestWS++; c = *sPtr++; }
    
    if (c == '\'' || c == '\"') { openQuote++; c = *sPtr++; }
    
    while (c != '\'' && c != '\"') { linkTitle++; c = *sPtr++; }
    
    if (c == '\'' || c == '\"') closeQuote++;
    
    linkLabelStr = create_substr(s, indentation + openBracket, indentation + openBracket + linkLabel);
    linkurlStr   = create_substr(s, 
        indentation + openBracket + linkLabel + closeBracket + colon + preDestWS, 
        indentation + openBracket + linkLabel + closeBracket + colon + preDestWS + destURL);
    
    if (linkTitle > 0)
    {
        linkTitleStr = create_substr(s,
            indentation + openBracket + linkLabel + closeBracket + colon + preDestWS + destURL + postDestWS + openQuote,
            indentation + openBracket + linkLabel + closeBracket + colon + preDestWS + destURL + postDestWS + openQuote + linkTitle);
    }
    
    return alloc_parse_info(LINK_REF_DEFINITION, 0, 0, INSERT_LINK);
}


/* [static] alloc_parse_info(mdblock_t, size_t, size_t, insert_t)
 * =======================================================================
 * Allocate space for a ParseInfo structure, initialize it with the 
 * mdblock_t and two sizes that were passed to the function.
 *
 * Return a pointer to the newly initializes ParseInfo structure.
 * NOTE: Function will exit program if the malloc call returns NULL.
 * ======================================================================= */
static ParseInfo *alloc_parse_info(mdblock_t type, size_t start, size_t stop, insert_t insert)
{
    ParseInfo *parse = malloc(sizeof(ParseInfo));
    if (parse)
    {
        parse->type   = type;
        parse->start  = start;
        parse->stop   = stop;
        parse->insert = insert;
        return parse;
    }
    else
    {
        atexit(print_memory_error);
        exit(EXIT_FAILURE);
    }
}


/* [static] reset_global_variables(ParseInfo *)
 * =======================================================================
 * Reset and/or update the global variables local to this file based on 
 * the information obtained by parsing. Then free the structure that holds
 * that information.
 * ======================================================================= */
static void reset_parsing_variables(ParseInfo *parse)
{    
    lastBlock    = parse->type;
    linkLabelStr = NULL;
    linkurlStr   = NULL;
    linkLabelStr = NULL;
    
    free(parse);
}


/* [static] check_last_block_type(char *)
 * =======================================================================
 * Check the mdblock_t of the last block parsed to determine if we already
 * know what this block will be, or if we can make a prediction about its
 * mdblock_t. This function should ideally be called before we begin 
 * parsing, and only continue parsing if we return NULL.
 *
 * Return a ParseInfo structure of information about the block, or return
 * null to let the parser know we must do some more work.
 * ======================================================================= */
static ParseInfo *check_last_block_type(char *line)
{
    ParseInfo *parse = NULL;
    
    if (strlen(line) == 0) return alloc_parse_info(BLANK_LINE, 0, 0, PLACEHOLDER);
    
    switch (lastBlock)
    {
        case BLANK_LINE:
        case PARAGRAPH:
            return NULL;
        case FENCED_CODE_BLOCK:
        case FENCED_CODE_BLOCK_START:
            return parse_code_fence(line, lastFencedBlockMark);
        case HTML_BLOCK:
            return parse_html_block(line);
        default: 
            return NULL;
    }
}