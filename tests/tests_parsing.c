/* tests_parsing.c
 *
 * ~~~~~~ultralightbeams~~~~~~~
 *  AUTHOR: Pat Gaffney       *
 *   EMAIL: <pat@hypepat.com> *
 *    DATE: 03/26/2016        *
 * PROJECT: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * =======================================================================
 * This file tests all the parsing functions to determine if they are 
 * operating on and returning data correctly.
 * ======================================================================= */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../parsers.h"
#include "../markdown.h"
#include "tests.h"
#include "tests_parsing.h"

/* Static local functions for this file only.
 * ======================================================================= */
static void atx_heading_tests(void);
static void horizontal_rule_tests(void);
static void setext_heading_tests(void);
static void indented_code_block_tests(void);
static void clear_parsing_variables(void);
static void test_parser(char *line, const mdblock_t returnType, const char *returnString);


/* Global variable to hold string values of each mdblock_t.
 * ======================================================================= */
const char *blockTypes[] = 
{
    "UNKNOWN",
    "BLANK_LINE",
    "ATX_HEADING_1",
    "ATX_HEADING_2",
    "ATX_HEADING_3",
    "ATX_HEADING_4",
    "ATX_HEADING_5",
    "ATX_HEADING_6",
    "INDENTED_CODE_BLOCK",
    "PARAGRAPH",
    "SETEXT_HEADING_1",
    "SETEXT_HEADING_2",
    "HORIZONTAL_RULE",
    "FENCED_CODE_BLOCK_START",
    "FENCED_CODE_BLOCK",
    "FENCED_CODE_BLOCK_STOP",
    "HTML_BLOCK",
    "LINK_REF_DEFINITION",
    "BLOCK_COMMENT",
    "BLOCKQUOTE_START",
    "BLOCKQUOTE_TEXT",
    "BLOCKQUOTE_END",
    "UNORDERED_LIST_START",
    "UNORDERED_LIST_ITEM",
    "UNORDERED_LIST_END",
    "ORDERED_LIST_START",
    "ORDERED_LIST_ITEM",
    "ORDERED_LIST_END"
};

/* main_parsing()
 * =======================================================================
 * This function calls all the other functions that perform the actual
 * parsing tests. This function is the only one that can be called by 
 * functions in other files.
 * ======================================================================= */
void main_parsing(void)
{
    printf("\n%sHorizontal Rules:%s\n", colors.bold, colors.reset);
    horizontal_rule_tests();
    
    printf("\n%sATX Headers:%s\n", colors.bold, colors.reset);
    atx_heading_tests();
    
    printf("\n%sSetext Headers:%s\n", colors.bold, colors.reset);
    setext_heading_tests();
    
    printf("\n%sIndented Code Blocks:%s\n", colors.bold, colors.reset);
    indented_code_block_tests();
}


/* [static] horizontal_rule_tests()
 * =======================================================================
 * Test the parse_horizontal_rule() function primarily, while checking 
 * edge cases for HORIZONTAL_RULEs.
 * ======================================================================= */
static void horizontal_rule_tests(void)
{
    clear_parsing_variables();
    if (tests.verbose)
    {
        printf("\n   A line consisting of three or more matching \'-\', \'_\', or \'*\'");
        printf("\n   characters forms a horizontal rule. The string is parsed into a single");
        printf("\n   character, as it is just a placerholder element, it is never written to");
        printf("\n   the output file.\n\n");
    }
    test_parser("***", HORIZONTAL_RULE, "*");
    test_parser("---", HORIZONTAL_RULE, "-");
    test_parser("___", HORIZONTAL_RULE, "_");
    test_parser("===", PARAGRAPH, "===");
    test_parser("+++", PARAGRAPH, "+++");
    
    if (tests.verbose)
    {
        printf("\n   If less than 3 characters are provided, the line is a paragraph.\n\n");
    }
    clear_parsing_variables();
    test_parser("--", PARAGRAPH, "--");
    test_parser("__", PARAGRAPH, "__");
    test_parser("**", PARAGRAPH, "**");
    
    if (tests.verbose)
    {
        printf("\n   One to three spaces of indentation is allowed, four spaces and the line");
        printf("\n   will be parsed as an INDENTED_CODE_BLOCK.\n\n");
    }
    test_parser(" ***", HORIZONTAL_RULE, " ");
    test_parser("  ***", HORIZONTAL_RULE, " ");
    test_parser("   ***", HORIZONTAL_RULE, " ");
    test_parser("    ***", INDENTED_CODE_BLOCK, "***");
    
    if (tests.verbose)
    {
        printf("\n   More than 3 characters may be used.\n\n");
    }
    test_parser("--------------", HORIZONTAL_RULE, "-");
    
    if (tests.verbose)
    {
        printf("\n   Spaces are allowed anywhere between or after the characters.\n\n");
    }
    test_parser(" - - -", HORIZONTAL_RULE, " ");
    test_parser(" **  * ** *", HORIZONTAL_RULE, " ");
    test_parser("-   -    -", HORIZONTAL_RULE, "-");
    
    if (tests.verbose)
    {
        printf("\n   No other characters may occur inline. It is also required that all");
        printf("\n   non-whitespace characters be the same.\n\n");
    }
    test_parser("___a", PARAGRAPH, "___a");
    test_parser("a------", PARAGRAPH, "a------");
    test_parser("---a---", PARAGRAPH, "---a---");
    test_parser("-*-*", PARAGRAPH, "-*-*");
    
    if (tests.verbose)
    {
        printf("\n   If there is any discrepancy in whether the line should be parsed as");
        printf("\n   a horizontal rule or a setext heading, the setext heading takes precedence.\n\n");
    }
    test_parser("paragraph", PARAGRAPH, "paragraph");
    test_parser("----", SETEXT_HEADING_2, "-");
}


/* [static] atx_heading_tests()
 * =======================================================================
 * Test the parse_atx_heading() function primarily, while checking 
 * edge cases for ATX_HEADING_x's.
 * ======================================================================= */
static void atx_heading_tests(void)
{
    clear_parsing_variables();
    if (tests.verbose)
    {
        printf("\n   Simple ATX Headings consist of 1-6 unescaped \'#\' characters. If more");
        printf("\n   than 6 \'#\' characters start a line, then the line is a PARAGRAPH.\n\n");
    }
    test_parser("# heading", ATX_HEADING_1, "heading");
    test_parser("## heading", ATX_HEADING_2, "heading");
    test_parser("### heading", ATX_HEADING_3, "heading");
    test_parser("#### heading", ATX_HEADING_4, "heading");
    test_parser("##### heading", ATX_HEADING_5, "heading");
    test_parser("###### heading", ATX_HEADING_6, "heading");
    test_parser("####### heading", PARAGRAPH, "####### heading");
    
    if (tests.verbose)
    {
        printf("\n   The opening sequence of \'#\' characters must be followed by a space");
        printf("\n   or the line will be considered a PARAGRAPH.\n\n");
    }
    test_parser("#5 bolt", PARAGRAPH, "#5 bolt");
    test_parser("#hashtag", PARAGRAPH, "#hashtag");
    
    if (tests.verbose)
    {
        printf("\n   If the openeing sequence of \'#\' characters is escaped with a \'\\\',");
        printf("\n   the line will be considered a PARAGRAPH.\n\n");
    }
    test_parser("\\## foo", PARAGRAPH, "\\## foo");
    
    if (tests.verbose)
    {
        printf("\n   Any spaces after the opening sequence of \'#\' characters and the first");
        printf("\n   non-whitespace character are ignored. Any spaces after the last heading");
        printf("\n   character are also ignored.\n\n");
    }
    test_parser("#   foo    ", ATX_HEADING_1, "foo");
    
    if (tests.verbose)
    {
        printf("\n   Up to 3 spaces of indentation are allowed. Any more than that and the line");
        printf("\n   will be parsed as an INDENTED_CODE_BLOCK.\n\n");
    }
    test_parser(" ### foo", ATX_HEADING_3, "foo");
    test_parser("  ## foo", ATX_HEADING_2, "foo");
    test_parser("   # foo", ATX_HEADING_1, "foo");
    test_parser("    # foo", INDENTED_CODE_BLOCK, "# foo");
    
    if (tests.verbose)
    {
        printf("\n   A closing sequence of \'#\' characters are allowed, they need not be the");
        printf("\n   same length as the opening sequence. The level of the ATX heading is");
        printf("\n   determined from the opening sequence only.\n\n");
    }
    test_parser("### foo ###", ATX_HEADING_3, "foo");
    test_parser("# foo ######", ATX_HEADING_1, "foo");
    test_parser("###### foo #", ATX_HEADING_6, "foo");
    test_parser("## foo ##   ", ATX_HEADING_2, "foo");
    
    if (tests.verbose)
    {
        printf("\n   If a non-whitespace and non-# character is found after the closing seqnece");
        printf("\n   of \'#\' characters then the closing sequence will be considered part of ");
        printf("\n   the heading, and will not be removed.\n\n");
    }
    test_parser("### foo ### b", ATX_HEADING_3, "foo ### b");
    
    if (tests.verbose)
    {
        printf("\n   ATX headings can be empty.\n\n");
    }
    test_parser("## ", ATX_HEADING_2, "");
    test_parser("## ##", ATX_HEADING_2, "");
}

/* [static] setext_headings_tests()
 * =======================================================================
 * Test the parse_setext_heading() function primarily, while checking the
 * edge cases for SETEXT_HEADING_x's.
 * ======================================================================= */
static void setext_heading_tests(void)
{
    clear_parsing_variables();
    if (tests.verbose)
    {
        printf("\n   A setext heading turns the previously parsed block element, which must");
        printf("\n   be a PARAGRAPH, into a <h1> or <h2> element. The line following the");
        printf("\n   PARAGRAPH must be a setext underline, which consists of a sequence of");
        printf("\n   \'=\' or \'-\' characters. Setext headings are not added as block");
        printf("\n   level elements, they just update the mdblock_t of the previously element.\n\n");
    }
    test_parser("paragraph", PARAGRAPH, "paragraph");
    test_parser("=====", SETEXT_HEADING_1, "=");
    test_parser("paragraph", PARAGRAPH, "paragraph");
    test_parser("-----", SETEXT_HEADING_2, "-");
    
    if (tests.verbose)
    {
         printf("\n   Because multi-line PARAGRAPHs are appended to become one block-level element,");
         printf("\n   any PARAGRAPH element that is followed by a setext underline will become a");
         printf("\n   SETEXT_HEADING_x. This means that the content of a SETEXT_HEADING_x may span");
         printf("\n   more than one line.\n\n");
    }
    test_parser("first line", PARAGRAPH, "first line");
    test_parser("second line", PARAGRAPH, "second line");
    test_parser("===", SETEXT_HEADING_1, "=");
    
    if (tests.verbose)
    {
        printf("\n   The setext underline can be of any length.\n\n");
    }
    test_parser("paragraph", PARAGRAPH, "paragraph");
    test_parser("===============", SETEXT_HEADING_1, "=");
    test_parser("paragraph", PARAGRAPH, "paragraph");
    test_parser("-", SETEXT_HEADING_2, "-");
    
    if (tests.verbose)
    {
        printf("\n   The setext underline can be indented up to 3 spaces, any more than that, and");
        printf("\n   it will be appended to the PARAGRAPH.\n\n");
    }
    test_parser("paragraph", PARAGRAPH, "paragraph");
    test_parser("    ----", PARAGRAPH, "----");
    
    if (tests.verbose)
    {
        printf("\n   The setext underline cannot contain internal spaces.\n\n");
    }
    test_parser("paragraph", PARAGRAPH, "paragraph");
    test_parser(" = = = = ", PARAGRAPH, "= = = = ");
}


/* [static] setext_headings_tests()
 * =======================================================================
 * Test the parse_setext_heading() function primarily, while checking the
 * edge cases for SETEXT_HEADING_x's.
 * ======================================================================= */
static void indented_code_block_tests(void)
{
    clear_parsing_variables();
    if (tests.verbose)
    {
        printf("\n   An indented code block is one or more consecutive lines of indented chunks. Each");
        printf("\n   line must be indented at least 4 spaces. The indentation will be removed during");
        printf("\n   parsing, up to 4 spaces.\n\n");
    }
    test_parser("    a simple", INDENTED_CODE_BLOCK, "a simple");
    test_parser("    indented", INDENTED_CODE_BLOCK, "indented");
    test_parser("       code", INDENTED_CODE_BLOCK, "   code");
    test_parser("    block", INDENTED_CODE_BLOCK, "block");
    clear_parsing_variables();
    
    if (tests.verbose)
    {
        printf("\n   An indented code block cannot interrupt a paragraph, so there must be a blank");
        printf("\n   line between a paragraph and an indented code block.\n\n");
    }
    test_parser("a paragraph", PARAGRAPH, "a paragraph");
    test_parser("    not code block", PARAGRAPH, "not code block");
    clear_parsing_variables();
    
    if (tests.verbose)
    {
        printf("\n   Any non-blank line with fewer than 4 spaces of indentation ends the code block,");
        printf("\n   that line is then parsed as a fresh block.\n\n");
    }
    test_parser("    code", INDENTED_CODE_BLOCK, "code");
    test_parser("foo", PARAGRAPH, "foo");
    clear_parsing_variables();
    
    if (tests.verbose)
    {
        printf("\n   Indented code can occur immediately before or after any block that is not a ");
        printf("\n   PARAGRAPH.\n\n");
    }
    test_parser("# heading", ATX_HEADING_1, "heading");
    test_parser("    code", INDENTED_CODE_BLOCK, "code");
    test_parser("------", HORIZONTAL_RULE, "-");
    test_parser("    code", INDENTED_CODE_BLOCK, "code");
    test_parser("------", HORIZONTAL_RULE, "-");
    test_parser("    code", INDENTED_CODE_BLOCK, "code");
    clear_parsing_variables();
    
    if (tests.verbose)
    {
        printf("\n   The first line of an indented code block can be indented more than four spaces.\n\n");
    }
    test_parser("      code", INDENTED_CODE_BLOCK, "  code");
    test_parser("    code2", INDENTED_CODE_BLOCK, "code2");
    clear_parsing_variables();
    
    if (tests.verbose)
    {
        printf("\n   Trailing spaces are included in the code blocks content.\n\n");
    }
    test_parser("    code  ", INDENTED_CODE_BLOCK, "code  ");
}


/* [static] test_parser(char *, const mdblock_t, const char *)
 * =======================================================================
 * Call parse_block_type() with the string passed to the function, check
 * the return TempMarkdownBlock against the mdblock_t and returnString
 * passed to the function. Print the results.
 * ======================================================================= */
static void test_parser(char *line, const mdblock_t returnType, const char *returnString)
{
    TempMarkdownBlock *block = parse_block_type(line);
    
    if (block->blockType == returnType && strcmp(returnString, block->blockString) == 0)
    {
        printf("   %s%s %s", colors.bold, colors.pass, colors.test);
    }
    else printf("   %s%s %s", colors.bold, colors.fail, colors.test);
    
    printf("\'%s\'", line);
    
    if (strlen(line) < 3) printf("\t\t\t");
    else if (strlen(line) < 11) printf("\t\t");
    else printf("\t");
    
    printf("is a %-20s--> becomes \'%s\'\n", blockTypes[block->blockType], block->blockString);
}


/* [static] clear_parsing_variables()
 * =======================================================================
 * Clear the parsing variables local to parsers.c that control how
 * certain block elements are parsed. This is accomplished by telling the
 * parser to parse a blank line.
 * ======================================================================= */
static void clear_parsing_variables(void)
{
    parse_block_type("");
}