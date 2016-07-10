/* 
 * tests.c -- unit tests for the parsers
 * 
 * Created by PAT GAFFNEY on 06/22/2016.
 * Last modified on 06/22/2016.
 * 
 *********ultrapatbeams*/

#include <stdio.h>
#include <string.h>
#include "parsers.h"
#include "markdown.h"

typedef struct {
    char *green;
    char *red;
    char *blue;
    char *magenta;
    char *reset;
    char *bold;
} colors_t;

colors_t clrs = {
    .green   = "\x1b[32m",
    .red     = "\x1b[31m",
    .blue    = "\x1b[34m",
    .magenta = "\x1b[35m",
    .reset   = "\x1b[0m",
    .bold    = "\x1b[1m"
};

static char *string[24] = {
    "UNKNOWN",
    "BLANK_LINE",
    "ATX_HEADER_1",
    "ATX_HEADER_2",
    "ATX_HEADER_3",
    "ATX_HEADER_4",
    "ATX_HEADER_5",
    "ATX_HEADER_6",
    "HORIZONTAL_RULE",
    "PARAGRAPH",
    "SETEXT_HEADER_1",
    "SETEXT_HEADER_2",
    "INDENTED_CODE_BLOCK",
    "BLOCKQUOTE_START",
    "BLOCKQUOTE_END",
    "FENCED_CODE_BLOCK",
    "UNORDERED_LIST_START",
    "UNORDERED_LIST_ITEM",
    "UNORDERED_LIST_END",
    "ORDERED_LIST_START",
    "ORDERED_LIST_ITEM",
    "ORDERED_LIST_END",
    "LINK_REFERENCE_DEF",
    "HTML_BLOCK"
};

static size_t passed = 0;
static size_t failed = 0;

static void clear_parser(void)
{
    block_parser("");
}


static bool run_test(mdblock_t type, char *raw, char *parsed)
{
    markdown_t *node = block_parser(raw);
    bool passed = false;
    
    if (!parsed) {
        if (node->type == type) {
            printf("%sPASSED:%s \'%s\' -> %s\n", clrs.green, clrs.reset, raw, string[node->type]);
            passed = true;
        }
        else {
            printf("%sFAILED:%s \'%s\' -> %s\n", clrs.red, clrs.reset, raw, string[node->type]);
        }
    }
    else {
        if (node->type == type && strcmp(parsed, node->value) == 0) {
            printf("%sPASSED:%s \'%s\' -> \'%s\' as %s\n", clrs.green, 
                   clrs.reset, raw, node->value, string[node->type]);
            passed = true;
        }
        else {
            printf("%sFAILED:%s \'%s\' -> \'%s\' as %s\n", clrs.red, 
                   clrs.reset, raw, node->value, string[node->type]);
        }
    }
    return passed;
}

void test_atx_headers(void)
{
    // correct usage
    run_test(ATX_HEADER_1, "# heading", "heading") ? passed++ : failed++;
    run_test(ATX_HEADER_2, "## heading", "heading") ? passed++ : failed++;
    run_test(ATX_HEADER_3, "### heading", "heading") ? passed++ : failed++;
    run_test(ATX_HEADER_4, "#### heading", "heading") ? passed++ : failed++;
    run_test(ATX_HEADER_5, "##### heading", "heading") ? passed++ : failed++;
    run_test(ATX_HEADER_6, "###### heading", "heading") ? passed++ : failed++;
    
    // more than 6 hashes is a paragraph
    run_test(PARAGRAPH, "####### heading", "####### heading") ? passed++ : failed++;
    
    // at least 1 space required between last hash and start of heading
    run_test(PARAGRAPH, "#5 bolt", "#5 bolt") ? passed++ : failed++;
    run_test(PARAGRAPH, "  #hastag", "#hastag") ? passed++ : failed++;
    
    // escaped headings parse as paragraphs
    run_test(PARAGRAPH, "\\## foo", "\\## foo") ? passed++ : failed++; // fix with inlines
    
    // leading and trailing blanks are ignored
    run_test(ATX_HEADER_1, "#        foo         ", "foo") ? passed++ : failed++;
    
    // 1 to 3 leading spaces is allowed -- 4 is an INDENTED_CODE_BLOCK
    run_test(ATX_HEADER_3, " ### foo", "foo") ? passed++ : failed++;
    run_test(ATX_HEADER_2, "  ## foo", "foo") ? passed++ : failed++;
    run_test(ATX_HEADER_1, "   # foo", "foo") ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "    # foo", "# foo") ? passed++ : failed++;
    
    // closing sequence of hashes is optional
    run_test(ATX_HEADER_2, "## foo ##", "foo") ? passed++ : failed++;
    run_test(ATX_HEADER_3, "  ###   bar    ###", "bar") ? passed++ : failed++;
    
    // closing sequence can be of any length
    run_test(ATX_HEADER_1, "# foo #############", "foo") ? passed++ : failed++;
    run_test(ATX_HEADER_5, "##### foo ##", "foo") ? passed++ : failed++;
    
    // trailing spaces are allowed
    run_test(ATX_HEADER_3, "### foo ###     ", "foo") ? passed++ : failed++;
    
    // any non-WS characters after the trailing hashes becomes part of the heading
    run_test(ATX_HEADER_3, "### foo ### b", "foo ### b") ? passed++ : failed++;
    
    // the closing sequence must be preceded by a space
    run_test(ATX_HEADER_1, "# foo#", "foo#") ? passed++ : failed++;
}

void test_horizontal_rule(void)
{
    // correct characters
    run_test(HORIZONTAL_RULE, "***", NULL) ? passed++ : failed++;
    run_test(HORIZONTAL_RULE, "---", NULL) ? passed++ : failed++;
    run_test(HORIZONTAL_RULE, "___", NULL) ? passed++ : failed++;
    
    // wrong characters
    run_test(PARAGRAPH, "===", "===") ? passed++ : failed++;
    clear_parser();
    run_test(PARAGRAPH, "+++", "+++") ? passed++ : failed++;
    clear_parser();
    
    // not enough characters
    run_test(PARAGRAPH, "**", "**") ? passed++ : failed++;
    clear_parser();
    run_test(PARAGRAPH, "--", "--") ? passed++ : failed++;
    clear_parser();
    run_test(PARAGRAPH, "__", "__") ? passed++ : failed++;
    
    // one to three spaces -- four is too many
    run_test(HORIZONTAL_RULE, " ***", NULL) ? passed++ : failed++;
    run_test(HORIZONTAL_RULE, "  ***", NULL) ? passed++ : failed++;
    run_test(HORIZONTAL_RULE, "   ***", NULL) ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "    ***", "***") ? passed++ : failed++;
    clear_parser();
    
    // more than three characters may be used
    run_test(HORIZONTAL_RULE, "-------------------", NULL) ? passed++ : failed++;
    
    // spaces are allowed between the characters
    run_test(HORIZONTAL_RULE, " - - -", NULL) ? passed++ : failed++;
    run_test(HORIZONTAL_RULE, " **  * ** * ** * **", NULL) ? passed++ : failed++;
    run_test(HORIZONTAL_RULE, "-     -      -      -", NULL) ? passed++ : failed++;
    
    // spaces are allowed at the end
    run_test(HORIZONTAL_RULE, "- - - -    ", NULL) ? passed++ : failed++;
    
    // no other characters can occur inline
    run_test(PARAGRAPH, "_ _ _ _ a", "_ _ _ _ a") ? passed++ : failed++;
    clear_parser();
    run_test(PARAGRAPH, "a------", "a------") ? passed++ : failed++;
    clear_parser();
    run_test(PARAGRAPH, "---a---", "---a---") ? passed++ : failed++;
    clear_parser();
    
    // all non-WS characters must be the same
    run_test(PARAGRAPH, " *-*", "*-*") ? passed++ : failed++;
    clear_parser();
    
    // can interrupt a paragraph
    run_test(PARAGRAPH, "start", "start") ? passed++ : failed++;
    run_test(HORIZONTAL_RULE, "***", NULL) ? passed++ : failed++;
    run_test(PARAGRAPH, "stop", "stop") ? passed++ : failed++;
    clear_parser();
    
    // setext heading takes precedence
    run_test(PARAGRAPH, "paragraph", "paragraph") ? passed++ : failed++;
    run_test(SETEXT_HEADER_2, "---", NULL) ? passed++ : failed++;
    clear_parser();
}

void test_setext_headers(void)
{
    // correct uses
    run_test(PARAGRAPH, "paragraph", "paragraph") ? passed++ : failed++;
    run_test(SETEXT_HEADER_1, "========", NULL) ? passed++ : failed++;
    run_test(PARAGRAPH, "paragraph", "paragraph") ? passed++ : failed++;
    run_test(SETEXT_HEADER_2, "--------", NULL) ? passed++ : failed++;
    
    // content may span multiple lines
    run_test(PARAGRAPH, "paragraph", "paragraph") ? passed++ : failed++;
    run_test(PARAGRAPH, "another paragraph", "another paragraph") ? passed++ : failed++;
    run_test(SETEXT_HEADER_1, "====", NULL) ? passed++ : failed++;
    
    // underline can be of any length
    run_test(PARAGRAPH, "paragraph", "paragraph") ? passed++ : failed++;
    run_test(SETEXT_HEADER_1, "=", NULL) ? passed++ : failed++;
    run_test(PARAGRAPH, "paragraph", "paragraph") ? passed++ : failed++;
    run_test(SETEXT_HEADER_2, "---------------------", NULL) ? passed++ : failed++;
    
    // heading content (paragraph) need not line up with the leading spaces of underlining
    run_test(PARAGRAPH, "   paragraph", "paragraph") ? passed++ : failed++;
    run_test(SETEXT_HEADER_2, "  ----", NULL) ? passed++ : failed++;
    
    // four spaces is too much
    run_test(INDENTED_CODE_BLOCK, "    code", "code") ? passed++ : failed++;
    run_test(HORIZONTAL_RULE, "-----", NULL) ? passed++ : failed++;
    
    // setext heading can have up to three spaces of underlining
    run_test(PARAGRAPH, "paragraph", "paragraph") ? passed++ : failed++;
    run_test(SETEXT_HEADER_1, "   =", NULL) ? passed++ : failed++;
    
    // four spaces is too much
    run_test(PARAGRAPH, "paragraph", "paragraph") ? passed++ : failed++;
    run_test(PARAGRAPH, "       =", " =") ? passed++ : failed++;
}

int main(int argc, char const **argv)
{
    printf("Begin running tests...\n\n");
    printf("%sATX HEADERS:%s\n", clrs.bold, clrs.reset);
    test_atx_headers();
    printf("\n%sHORIZONTAL RULES:%s\n", clrs.bold, clrs.reset);
    test_horizontal_rule();
    printf("\n%sSETEXT HEADERS:%s\n", clrs.bold, clrs.reset);
    test_setext_headers();
    
    printf("\n\n%sTOTAL TESTS: %zu%s\n", clrs.bold, passed + failed, clrs.reset);
    if (failed == 0) printf("%s%sPASSED ALL %zu TESTS!%s\n", clrs.bold, clrs.green, passed, clrs.reset);
    else {
        printf("%s%s%zu TEST(S) FAILED%s //", clrs.bold, clrs.red, failed, clrs.reset);
        printf("%s%s %zu TEST(S) PASSED %s\n", clrs.bold, clrs.green, passed, clrs.reset);
    }
    return 0;
}