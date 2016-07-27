/* 
 * tests.c -- unit tests for the parsers
 * 
 * Created by PAT GAFFNEY on 06/22/2016.
 * Last modified on 07/12/2016.
 * 
 *********ultrapatbeams*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parsers.h"
#include "markdown.h"
#include "strings.h"


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

static char *string[27] = {
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
    "FENCED_CODE_BLOCK_START",
    "FENCED_CODE_BLOCK",
    "FENCED_CODE_BLOCK_END",
    "HTML_BLOCK",
    "HTML_COMMENT",
    "BLOCKQUOTE_START",
    "BLOCKQUOTE_END",
    "UNORDERED_LIST_START",
    "UNORDERED_LIST_ITEM",
    "UNORDERED_LIST_END",
    "ORDERED_LIST_START",
    "ORDERED_LIST_ITEM",
    "ORDERED_LIST_END",
    "LINK_REFERENCE_DEF"
};

static size_t passed = 0;
static size_t failed = 0;

static void clear_parser(void)
{
    string_t *str = init_stringt(1);
    str->len = 0;
    str->string = "";
    block_parser(str);
}


static bool run_test(mdblock_t type, char *raw, char *parsed)
{
    string_t *str = init_stringt(strlen(raw));
    str->string   = raw;
    str->len      = strlen(raw);
    markdown_t *node = block_parser(str);
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
        if (node->type == type && strcmp(parsed, node->value->string) == 0) {
            printf("%sPASSED:%s \'%s\' -> \'%s\' as %s\n", clrs.green, 
                   clrs.reset, raw, node->value->string, string[node->type]);
            passed = true;
        }
        else {
            printf("%sFAILED:%s \'%s\' -> \'%s\' as %s\n", clrs.red, 
                   clrs.reset, raw, node->value->string, string[node->type]);
        }
    }
    if (node) free(node);
    if (str) free(str);
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
    run_test(PARAGRAPH, "       =", "=") ? passed++ : failed++;
    
    // the setext heading underline cannot contain internal spaces
    run_test(PARAGRAPH, "foo", "foo") ? passed++ : failed++;
    run_test(PARAGRAPH, "= =", "= =") ? passed++ : failed++;
    run_test(PARAGRAPH, "bar", "bar") ? passed++ : failed++;
    run_test(PARAGRAPH, "--- -", "--- -") ? passed++ : failed++;
    clear_parser();
    
    // setext headings cannot be empty
    run_test(PARAGRAPH, "===", "===") ? passed++ : failed++;
    clear_parser();
    
    // multi-line paragraphs
    run_test(PARAGRAPH, "foo", "foo") ? passed++ : failed++;
    run_test(PARAGRAPH, "bar", "bar") ? passed++ : failed++;
    run_test(SETEXT_HEADER_2, "---", NULL) ? passed++ : failed++;
    run_test(PARAGRAPH, "bar", "bar") ? passed++ : failed++;
    clear_parser();
}

void test_indented_code_blocks(void)
{
    // correct uses
    run_test(INDENTED_CODE_BLOCK, "    code", "code") ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "      block", "  block") ? passed++ : failed++;
    clear_parser();
    
    // any more than four spaces are included in the content
    run_test(INDENTED_CODE_BLOCK, "    main() {", "main() {") ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "        int z = 8;", "    int z = 8;") ? passed++ : failed++;
    clear_parser();
    
    // separate chunks can be separated by blank lines -- as long as they are indented
    run_test(INDENTED_CODE_BLOCK, "    z = 33;", "z = 33;") ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "    ", "") ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "    print(z);", "print(z);") ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "    ", "") ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "      ", "  ") ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "    exit();", "exit();") ? passed++ : failed++;
    clear_parser();
    
    // indented code blocks cannot interrupt paragraphs
    run_test(PARAGRAPH, "foo", "foo") ? passed++ : failed++;
    run_test(PARAGRAPH, "    code?", "code?") ? passed++ : failed++;
    clear_parser();
    
    // any non-blank line with < 4 spaces of indentation ends a code block
    run_test(INDENTED_CODE_BLOCK, "    code", "code") ? passed++ : failed++;
    run_test(PARAGRAPH, "foo", "foo") ? passed++ : failed++;
    clear_parser();
    run_test(INDENTED_CODE_BLOCK, "    code", "code") ? passed++ : failed++;
    run_test(ATX_HEADER_1, "# heading", "heading") ? passed++ : failed++;
    clear_parser();
    
    // a code block can occur immediately after a heading
    run_test(ATX_HEADER_1, "# heading", "heading") ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "    code", "code") ? passed++ : failed++;
    clear_parser();
    run_test(PARAGRAPH, "foo", "foo") ? passed++ : failed++;
    run_test(SETEXT_HEADER_2, "---", NULL) ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "    code", "code") ? passed++ : failed++;
    clear_parser();
    
    // the first line can be indented more than four spaces
    run_test(INDENTED_CODE_BLOCK, "        code", "    code") ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "    code", "code") ? passed++ : failed++;
    clear_parser();
    
    // trailing spaces are included in the content
    run_test(INDENTED_CODE_BLOCK, "    code  ", "code  ") ? passed++ : failed++;
    clear_parser();
    
    // Blank lines preceding an indented code block are included
    run_test(BLANK_LINE, "    ", NULL) ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "    code", "code") ? passed++ : failed++;
    clear_parser();
}

void test_fenced_code_blocks(void)
{
    // basic usage with backticks
    run_test(FENCED_CODE_BLOCK_START, "```", NULL) ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "code", "code") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "    more code", "    more code") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "```", NULL) ? passed++ : failed++;
    clear_parser();
    
    // basic usage with tildas
    run_test(FENCED_CODE_BLOCK_START, "~~~", NULL) ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "code", "code") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "    more code", "    more code") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "~~~", NULL) ? passed++ : failed++;
    clear_parser();
    
    // closing code fence must use the same character as the opening fence
    run_test(FENCED_CODE_BLOCK_START, "```", NULL) ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "~~~", "~~~") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "```", NULL) ? passed++ : failed++;
    clear_parser();
    run_test(FENCED_CODE_BLOCK_START, "~~~", NULL) ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "```", "```") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "~~~", NULL) ? passed++ : failed++;
    clear_parser();
    
    // closing code fence must be at least as long as the opening fence
    run_test(FENCED_CODE_BLOCK_START, "`````", NULL) ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "```", "```") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "`````", NULL) ? passed++ : failed++;
    clear_parser();
    run_test(FENCED_CODE_BLOCK_START, "~~~", NULL) ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "~~~~~~~", "~~~~~~~") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "~~~", NULL) ? passed++ : failed++;
    clear_parser();
    
    // a code block can have all empty lines as its content
    run_test(FENCED_CODE_BLOCK_START, "~~~", NULL) ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "", "") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "~~~", NULL) ? passed++ : failed++;
    clear_parser();
    
    // a code block can be empty
    run_test(FENCED_CODE_BLOCK_START, "~~~", NULL) ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "~~~", NULL) ? passed++ : failed++;
    clear_parser();
    
    // fence code blocks can be indented up to four spaces
    // indentation of opening fence doesnt have to match closing fence
    run_test(FENCED_CODE_BLOCK_START, "  ~~~", NULL) ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "~~~", NULL) ? passed++ : failed++;
    clear_parser();
    
    // more than four spaces is an indented code block
    run_test(INDENTED_CODE_BLOCK, "    ~~~", NULL) ? passed++ : failed++;
    run_test(INDENTED_CODE_BLOCK, "    ~~~", NULL) ? passed++ : failed++;
    clear_parser();
    
    // fenced code blocks can interrupt paragraphs 
    // and can be followed directly by paragraphs
    run_test(PARAGRAPH, "paragraph", "paragraph") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_START, "~~~", NULL) ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "code", "code") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "~~~", NULL) ? passed++ : failed++;
    run_test(PARAGRAPH, "paragraph", "paragraph") ? passed++ : failed++;
    
    // an info string can be provided after the opening fence
    run_test(FENCED_CODE_BLOCK_START, "```ruby", "ruby") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "def foo(x)", "def foo(x)") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "```", NULL) ? passed++ : failed++;
    
    // only the first word of an info-string is saved
    run_test(FENCED_CODE_BLOCK_START, "```ruby startline=3", "ruby") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "def foo(x)", "def foo(x)") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "```", NULL) ? passed++ : failed++;
    
    // closing fences cannot have info-strings
    run_test(FENCED_CODE_BLOCK_START, "```ruby     ", "ruby") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK, "```ruby     ", "```ruby     ") ? passed++ : failed++;
    run_test(FENCED_CODE_BLOCK_END, "```", NULL) ? passed++ : failed++;
}

void test_paragraphs(void)
{
    // normal use
    run_test(PARAGRAPH, "aaa", "aaa") ? passed++ : failed++;
    run_test(BLANK_LINE, "", NULL) ? passed++ : failed++;
    run_test(PARAGRAPH, "bbb", "bbb") ? passed++ : failed++;
    
    // paragraphs can span multiple lines
    run_test(PARAGRAPH, "first", "first") ? passed++ : failed++;
    run_test(PARAGRAPH, "still first", "still first") ? passed++ : failed++;
    
    // leading spaces are skipped
    run_test(PARAGRAPH, "  aa", "aa") ? passed++ : failed++;
    run_test(PARAGRAPH, " b", "b") ? passed++ : failed++;
    
    // lines after the first can be indented any amount, since 
    // indented code blocks cannot interrupt paragraphs
    run_test(PARAGRAPH, "aaa", "aaa") ? passed++ : failed++;
    run_test(PARAGRAPH, "             bbb", "bbb") ? passed++ : failed++;
    run_test(PARAGRAPH, "        ccc", "ccc") ? passed++ : failed++;
    clear_parser();
    
    // if the first line is indented more than four lines its an
    // indented code block
    run_test(INDENTED_CODE_BLOCK, "    aaa", "aaa") ? passed++ : failed++;
    run_test(PARAGRAPH, "bbb", "bbb") ? passed++ : failed++;
    clear_parser();
}


void test_html_blocks(void)
{
    // normal use -- html block with lines w/o HTML elements
    run_test(HTML_BLOCK, "<table>", "<table>") ? passed++ : failed++;
    run_test(HTML_BLOCK, "  <tr>", "  <tr>") ? passed++ : failed++;
    run_test(HTML_BLOCK, "    <td>", "    <td>") ? passed++ : failed++;
    run_test(HTML_BLOCK, "      hi", "      hi") ? passed++ : failed++;
    run_test(HTML_BLOCK, "    <td>", "    <td>") ? passed++ : failed++;
    run_test(HTML_BLOCK, "  <tr>", "  <tr>") ? passed++ : failed++;
    run_test(HTML_BLOCK, "<table>", "<table>") ? passed++ : failed++;
    run_test(BLANK_LINE, "", NULL) ? passed++ : failed++;
    run_test(PARAGRAPH, "okay", "okay") ? passed++ : failed++;
    clear_parser();
    
    // test fake len=1 element
    run_test(PARAGRAPH, "<l>para</l>", "<l>para</l>") ? passed++ : failed++;
    clear_parser();
    
    // test <dd>
    run_test(HTML_BLOCK, "<dd>def</dd>", "<dd>def</dd>") ? passed++ : failed++;
    clear_parser();
    
    // test <dl>
    run_test(HTML_BLOCK, "<dl>def</dl>", "<dl>def</dl>") ? passed++ : failed++;
    clear_parser();
    
    // test <dt>
    run_test(HTML_BLOCK, "<dt>def</dt>", "<dt>def</dt>") ? passed++ : failed++;
    clear_parser();
    
    // test <td>
    run_test(HTML_BLOCK, "<td>table</td>", "<td>table</td>") ? passed++ : failed++;
    clear_parser();
    
    // test <th>
    run_test(HTML_BLOCK, "<th>table</th>", "<th>table</th>") ? passed++ : failed++;
    clear_parser();
    
    // test <tr>
    run_test(HTML_BLOCK, "<tr>table</tr>", "<tr>table</tr>") ? passed++ : failed++;
    clear_parser();
    
    // test fake len=2 element
    run_test(PARAGRAPH, "<ll>para</ll>", "<ll>para</ll>") ? passed++ : failed++;
    clear_parser();
    
    // test <col>
    run_test(HTML_BLOCK, "<col>a</col>", "<col>a</col>") ? passed++ : failed++;
    clear_parser();
    
    // test <dir>
    run_test(HTML_BLOCK, "<dir>a</dir>", "<dir>a</dir>") ? passed++ : failed++;
    clear_parser();
    
    // test <div>
    run_test(HTML_BLOCK, "<div>a</div>", "<div>a</div>") ? passed++ : failed++;
    clear_parser();
    
    // test <nav>
    run_test(HTML_BLOCK, "<nav>a</nav>", "<nav>a</nav>") ? passed++ : failed++;
    clear_parser();
    
    // test fake len=3 element
    run_test(PARAGRAPH, "<nan>para</nan>", "<nan>para</nan>") ? passed++ : failed++;
    clear_parser();
    
    // test <base>
    run_test(HTML_BLOCK, "<base>", "<base>") ? passed++ : failed++;
    clear_parser();
    
    // test <body>
    run_test(HTML_BLOCK, "<body>", "<body>") ? passed++ : failed++;
    clear_parser();
    
    // test <form>
    run_test(HTML_BLOCK, "<form>", "<form>") ? passed++ : failed++;
    clear_parser();
    
    // test <head>
    run_test(HTML_BLOCK, "<head>", "<head>") ? passed++ : failed++;
    clear_parser();
    
    // test <html>
    run_test(HTML_BLOCK, "<html>", "<html>") ? passed++ : failed++;
    clear_parser();
    
    // test <link>
    run_test(HTML_BLOCK, "<link>", "<link>") ? passed++ : failed++;
    clear_parser();
    
    // test <main>
    run_test(HTML_BLOCK, "<main>", "<main>") ? passed++ : failed++;
    clear_parser();
    
    // test <menu>
    run_test(HTML_BLOCK, "<menu>", "<menu>") ? passed++ : failed++;
    clear_parser();
    
    // test <meta>
    run_test(HTML_BLOCK, "<meta>", "<meta>") ? passed++ : failed++;
    clear_parser();
    
    // test fake len=4 element
    run_test(PARAGRAPH, "<bold>", "<bold>") ? passed++ : failed++;
    clear_parser();
    
    // test <aside>
    run_test(HTML_BLOCK, "<aside>", "<aside>") ? passed++ : failed++;
    clear_parser();
    
    // test <frame>
    run_test(HTML_BLOCK, "<frame>", "<frame>") ? passed++ : failed++;
    clear_parser();
    
    // test <param>
    run_test(HTML_BLOCK, "<param>", "<param>") ? passed++ : failed++;
    clear_parser();
    
    // test <style>
    run_test(HTML_BLOCK, "<style>", "<style>") ? passed++ : failed++;
    clear_parser();
    
    // test <table>
    run_test(HTML_BLOCK, "<table>", "<table>") ? passed++ : failed++;
    clear_parser();
    
    // test <tbody>
    run_test(HTML_BLOCK, "<tbody>", "<tbody>") ? passed++ : failed++;
    clear_parser();
    
    // test <tfoot>
    run_test(HTML_BLOCK, "<tfoot>", "<tfoot>") ? passed++ : failed++;
    clear_parser();
    
    // test <thead>
    run_test(HTML_BLOCK, "<thead>", "<thead>") ? passed++ : failed++;
    clear_parser();
    
    // test <title>
    run_test(HTML_BLOCK, "<title>", "<title>") ? passed++ : failed++;
    clear_parser();
    
    // test <track>
    run_test(HTML_BLOCK, "<track>", "<track>") ? passed++ : failed++;
    clear_parser();
    
    // test fake len=5 element
    run_test(PARAGRAPH, "<fucky>", "<fucky>") ? passed++ : failed++;
    clear_parser();
    
    // test <center>
    run_test(HTML_BLOCK, "<center>", "<center>") ? passed++ : failed++;
    clear_parser();
    
    // test <dialog>
    run_test(HTML_BLOCK, "<dialog>", "<dialog>") ? passed++ : failed++;
    clear_parser();
    
    // test <figure>
    run_test(HTML_BLOCK, "<figure>", "<figure>") ? passed++ : failed++;
    clear_parser();
    
    // test <footer>
    run_test(HTML_BLOCK, "<footer>", "<footer>") ? passed++ : failed++;
    clear_parser();
    
    // test <header>
    run_test(HTML_BLOCK, "<header>", "<header>") ? passed++ : failed++;
    clear_parser();
    
    // test <iframe>
    run_test(HTML_BLOCK, "<iframe>", "<iframe>") ? passed++ : failed++;
    clear_parser();
    
    // test <legend>
    run_test(HTML_BLOCK, "<legend>", "<legend>") ? passed++ : failed++;
    clear_parser();
    
    // test <option>
    run_test(HTML_BLOCK, "<option>", "<option>") ? passed++ : failed++;
    clear_parser();
    
    // test <script>
    run_test(HTML_BLOCK, "<script>", "<script>") ? passed++ : failed++;
    clear_parser();
    
    // test <source>
    run_test(HTML_BLOCK, "<source>", "<source>") ? passed++ : failed++;
    clear_parser();
    
    // test fake len=6 element
    run_test(PARAGRAPH, "<fucker>", "<fucker>") ? passed++ : failed++;
    clear_parser();
    
    // test <address>
    run_test(HTML_BLOCK, "<address>", "<address>") ? passed++ : failed++;
    clear_parser();
    
    // test <article>
    run_test(HTML_BLOCK, "<article>", "<article>") ? passed++ : failed++;
    clear_parser();
    
    // test <basefont>
    run_test(HTML_BLOCK, "<basefont>", "<basefont>") ? passed++ : failed++;
    clear_parser();
    
    // test <caption>
    run_test(HTML_BLOCK, "<caption>", "<caption>") ? passed++ : failed++;
    clear_parser();
    
    // test <colgroup>
    run_test(HTML_BLOCK, "<colgroup>", "<colgroup>") ? passed++ : failed++;
    clear_parser();
    
    // test <details>
    run_test(HTML_BLOCK, "<details>", "<details>") ? passed++ : failed++;
    clear_parser();
    
    // test <fieldset>
    run_test(HTML_BLOCK, "<fieldset>", "<fieldset>") ? passed++ : failed++;
    clear_parser();
    
    // test <figcaption>
    run_test(HTML_BLOCK, "<figcaption>", "<figcaption>") ? passed++ : failed++;
    clear_parser();
    
    // test <frameset>
    run_test(HTML_BLOCK, "<frameset>", "<frameset>") ? passed++ : failed++;
    clear_parser();
    
    // test <menuitem>
    run_test(HTML_BLOCK, "<menuitem>", "<menuitem>") ? passed++ : failed++;
    clear_parser();
    
    // test <noframes>
    run_test(HTML_BLOCK, "<noframes>", "<noframes>") ? passed++ : failed++;
    clear_parser();
    
    // test <optgroup>
    run_test(HTML_BLOCK, "<optgroup>", "<optgroup>") ? passed++ : failed++;
    clear_parser();
    
    // test <section>
    run_test(HTML_BLOCK, "<section>", "<section>") ? passed++ : failed++;
    clear_parser();
    
    // test <summary>
    run_test(HTML_BLOCK, "<summary>", "<summary>") ? passed++ : failed++;
    clear_parser();
    
    // test fake large element
    run_test(PARAGRAPH, "<douchecanoe>", "<douchecanoe>") ? passed++ : failed++;
    clear_parser();
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
    printf("\n%sINDENTED CODE BLOCKS:%s\n", clrs.bold, clrs.reset);
    test_indented_code_blocks();
    printf("\n%sFENCED CODE BLOCKS:%s\n", clrs.bold, clrs.reset);
    test_fenced_code_blocks();
    printf("\n%sPARAGRAPHS:%s\n", clrs.bold, clrs.reset);
    test_paragraphs();
    printf("\n%sHTML BLOCKS:%s\n", clrs.bold, clrs.reset);
    test_html_blocks();
    
    printf("\n\n%sTOTAL TESTS: %zu%s\n", clrs.bold, passed + failed, clrs.reset);
    if (failed == 0) printf("%s%sPASSED ALL %zu TESTS!%s\n", clrs.bold, clrs.green, passed, clrs.reset);
    else {
        printf("%s%s%zu TEST(S) FAILED%s //", clrs.bold, clrs.red, failed, clrs.reset);
        printf("%s%s %zu TEST(S) PASSED %s\n", clrs.bold, clrs.green, passed, clrs.reset);
    }
    return 0;
}