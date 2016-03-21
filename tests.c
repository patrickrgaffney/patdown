/* tests.c
 *
 * ~~~~~~ultralightbeams~~~~~~~
 * Author:  Pat Gaffney       *
 * Email:   <pat@hypepat.com> *
 * Date:    02/20/2016        *
 * Project: patdown           *
 * ~~~~~~ultralightbeams~~~~~~~
 *
 * This is the entry point for the testing portion of patdown. This
 * file calls functions with canned input to test the output.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arguments.h"
#include "files.h"
#include "parsers.h"
#include "markdown.h"

/* Static function definitions */
static int testArguments(const char *argv[], const char argc, const argtype_t argt[]);
static int testFileOpening(const char fileName[], const argtype_t fileType, const int readwrite);
static int testBlockParsing(const char *string, temp_block_node_t **fakeBlock, const mdblock_t lastBlockType);


/*******************************************************************
 * Every test prints a string with the following format:
 *      _ TEST: string
 * Where `_` is either a '√' for PASS or a '✕' for FAIL and
 * `string` is a description of the test. If the test is PASSed,
 * the color of `_ TEST:` is changed to green. If the test is 
 * FAILed, the color is changed to red. Both color changes are 
 * set using ANSI color codes.
 ******************************************************************/
const char pass[]     = "\x1b[32m√";
const char fail[]     = "\x1b[31m✕";
const char test[]     = "TEST: \x1b[0m";
const char blue[]     = "\x1b[34m";
const char magenta[]  = "\x1b[35m";
const char reset[]    = "\x1b[0m";
const char bold[]     = "\x1b[1m";

int main(int argc, char const *argv[])
{   
    /* ==============================================================
     * == COMMAND-LINE ARGUMENTS
     * =========================================================== */
    
    printf("\n%sCommand-line arguments:%s\n\n", blue, reset);
    
    printf("\tTest various combinations of command-line arguments to determine\n");
    printf("\tif they are being parsed correctly. Each individual argument is\n");
    printf("\tgiven a type, or argt, and that is compared against the expected type.\n\n");
    
    const char *fakeArgv[11][7] = {
        {"patdown", "--help"},
        {"patdown", "--version"},
        {"patdown", "--help", "inFile"},
        {"patdown", "--version", "inFile"},
        {"patdown", "--version", "--help", "inFile"},
        {"patdown", "--version", "inFile", "--help"},
        {"patdown", "--version", "inFile", "--help", "-o", "outFile"},
        {"patdown", "--version", "inFile", "--help", "-f", "html"},
        {"patdown", "-f", "HTML", "inFile"},
        {"patdown", "-f", "txt", "--help"},
        {"patdown", "inFile", "-f", "TXT", "--help", "-o", "outFile"},
    };
    
    const argtype_t fakeArgt[11][7] = {
        {PROGRAM_NAME, HELP_FLAG},
        {PROGRAM_NAME, VERSION_FLAG},
        {PROGRAM_NAME, HELP_FLAG, INPUT_FILE_NAME},
        {PROGRAM_NAME, VERSION_FLAG, INPUT_FILE_NAME},
        {PROGRAM_NAME, VERSION_FLAG, HELP_FLAG, INPUT_FILE_NAME},
        {PROGRAM_NAME, VERSION_FLAG, INPUT_FILE_NAME, HELP_FLAG},
        {PROGRAM_NAME, VERSION_FLAG, INPUT_FILE_NAME, HELP_FLAG, OUTPUT_FILE_FLAG, OUTPUT_FILE_NAME},
        {PROGRAM_NAME, VERSION_FLAG, INPUT_FILE_NAME, HELP_FLAG, OUTPUT_FORMAT_FLAG, OUTPUT_FORMAT_NAME},
        {PROGRAM_NAME, OUTPUT_FORMAT_FLAG, OUTPUT_FORMAT_NAME, INPUT_FILE_NAME},
        {PROGRAM_NAME, OUTPUT_FORMAT_FLAG, OUTPUT_FORMAT_NAME, HELP_FLAG},
        {PROGRAM_NAME, INPUT_FILE_NAME, OUTPUT_FORMAT_FLAG, OUTPUT_FORMAT_NAME, HELP_FLAG, OUTPUT_FILE_FLAG, OUTPUT_FILE_NAME}
    };
    
    const char *stringArgv[11] = {
        "patdown --help",
        "patdown --version",
        "patdown --help inFile",
        "patdown --version inFile",
        "patdown --version --help inFile",
        "patdown --version inFile --help",
        "patdown --version inFile --help -o outFile",
        "patdown --version inFile --help -f html",
        "patdown -f HTML inFile",
        "patdown -f txt --help",
        "patdown --help inFile -f TXT --help -o outFile"
    };
    
    const int fakeArgc[11] = {2, 2, 3, 3, 4, 4, 6, 6, 4, 4, 7};
    
    for (size_t i = 0; i < 11; ++i)
    {
        printf("\t%s %s\'%s\'\n", testArguments(fakeArgv[i], fakeArgc[i], fakeArgt[i]) ? pass : fail, 
            test, stringArgv[i]);
    }
    
    /* ==============================================================
     * == FILE PROCESSING
     * =========================================================== */
     
    printf("\n");
    printf("%sFile-Processing:%s\n\n", blue, reset);
    
    // write a string to file
    const char string14[] = "write a string to output file \'test.md\'";
    printf("\t%s %s%s\n", testFileOpening("test.md", OUTPUT_FILE_NAME, 0) ? pass : fail, test, string14);
    
    // read string from a file
    const char string15[] = "read a string from input file \'test.md\'";
    printf("\t%s %s%s\n", testFileOpening("test.md", INPUT_FILE_NAME, 5) ? pass : fail, test, string15);
    
   /* ==============================================================
    * == MARKDOWN BLOCK PARSING
    * =========================================================== */
    
    printf("\n");
    printf("%sMarkdown Block Parsing:%s\n\n", blue, reset);
    
    temp_block_node_t *fakeBlock = malloc(sizeof(temp_block_node_t));
    mdblock_t lastBlockType;
    int returnValue = -1;
    
    char *returnTypes[] = {
        "UNKNOWN",
        "BLANK_LINE",
        "HORIZONTAL_RULE",
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
        "HTML_BLOCK",
        "HTML_COMMENT",
        "FENCED_CODE_BLOCK",
        "BLOCK_COMMENT",
        "LINK_REF_DEFINITION",
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
    
   /* ==============================================================
    * == HORIZONTAL RULES
    * =========================================================== */
    
    printf("\t%sHORIZONTAL RULES:%s\n\n", bold, reset);
    
    printf("\tA line consisting of 0-3 spaces of indentation, followed by a sequence\n");
    printf("\tof three or more matching %s-%s, %s_%s, or %s*%s characters, each followed by any\n",
        bold, reset, bold, reset, bold, reset);
    printf("\tnumber of spaces, forms a horizontal rule. \'NULL\' is the string returned\n");
    printf("\tfrom a valid horizontal rule, as it is never written to the output file.\n\n");
    
    /* TEST: "***" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("***", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[2], "***");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "---" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("---", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[2], "---");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "___" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("___", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[2], "___");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "===" */
    fakeBlock->blockString = "===";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("===", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "===");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "+++" */
    fakeBlock->blockString = "+++";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("+++", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "+++");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "--" */
    fakeBlock->blockString = "--";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("--", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "--");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "__" */
    fakeBlock->blockString = "__";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("__", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "__");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "**" */
    fakeBlock->blockString = "**";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("**", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "**");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tUp to 3 preceding spaces are allowed. More than 3 preceding spaces is an\n");
    printf("\tindented code block.\n\n");
    
    /* TEST: " ***" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing(" ***", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[2], " ***");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "  ***" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("  ***", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[2], "  ***");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "   ***" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("   ***", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[2], "   ***");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "    ***" */
    fakeBlock->blockString = "***";
    fakeBlock->blockType   = INDENTED_CODE_BLOCK;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("    ***", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[9], "    ***");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tMore than 3 characters may be used, and spaces are allowed between the\n");
    printf("\tcharacter as long as their is at least three of them.\n\n");
    
    /* TEST: "----------" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("----------", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[2], "----------");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: " - - -" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing(" - - -", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[2], " - - -");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: " **  * ** * ** * **" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing(" **  * ** * ** * **", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[2], " **  * ** * ** * **");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "-     -      -      -" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("-     -      -      -", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[2], "-     -      -      -");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tAn unlimited number of characters are allowed at the end of the string.\n\n");
    
    /* TEST: "___               " */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("___               ", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[2], "___               ");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tNo other characters may occur in the line, and this includes other\n");
    printf("\tpossible non-whitespace characters. In other words, all the non\n");
    printf("\twhitespace characters must be the same.\n\n");
    
    /* TEST: "a------" */
    fakeBlock->blockString = "a------";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("a------", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "a------");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "_ _ _ _ a" */
    fakeBlock->blockString = "_ _ _ _ a";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("_ _ _ _ a", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "_ _ _ _ a");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "---a---" */
    fakeBlock->blockString = "---a---";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("---a---", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "---a---");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: " *-*" */
    fakeBlock->blockString = "*-*";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing(" *-*", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], " *-*");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    
   /* ==============================================================
    * == ATX HEADERS
    * =========================================================== */
    
    printf("\t%sATX HEADERS:%s\n\n", bold, reset);
    
    printf("\tAn ATX heading consists of a string of characters between an opening\n");
    printf("\tsequence of 1-6 unescaped %s#%s characters and an optional sequence of\n",
        bold, reset);
    printf("\tunescaped %s#%s characters.\n\n", bold, reset);
    
    /* TEST: "# heading 1" */
    fakeBlock->blockString = "heading 1";
    fakeBlock->blockType   = ATX_HEADING_1;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("# heading 1", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[3], "# heading 1");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "## heading 2" */
    fakeBlock->blockString = "heading 2";
    fakeBlock->blockType   = ATX_HEADING_2;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("## heading 2", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[4], "## heading 2");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "### heading 3" */
    fakeBlock->blockString = "heading 3";
    fakeBlock->blockType   = ATX_HEADING_3;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("### heading 3", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[5], "### heading 3");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "#### heading 4" */
    fakeBlock->blockString = "heading 4";
    fakeBlock->blockType   = ATX_HEADING_4;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("#### heading 4", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[6], "#### heading 4");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "##### heading 5" */
    fakeBlock->blockString = "heading 5";
    fakeBlock->blockType   = ATX_HEADING_5;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("##### heading 5", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[7], "#### heading 5");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "###### heading 6" */
    fakeBlock->blockString = "heading 6";
    fakeBlock->blockType   = ATX_HEADING_6;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("###### heading 6", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[8], "###### heading 6");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "####### too many" */
    fakeBlock->blockString = "####### too many";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("####### too many", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "####### too many");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tAt least one space is required between the last %s#%s character and \n", bold, reset);
    printf("\tthe first character of the heading.\n\n");
    
    /* TEST: "#5 bolt" */
    fakeBlock->blockString = "#5 bolt";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("#5 bolt", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "#5 bolt");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "#hashtag" */
    fakeBlock->blockString = "#hashtag";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("#hashtag", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "#hashtag");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tA tab will not work for this required space.\n\n");
    
    /* TEST: "#\ttab" */
    fakeBlock->blockString = "#\ttab";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("#\ttab", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "#\ttab");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tThe first %s#%s character must not be escaped.\n\n", bold, reset);
    
    /* TEST: "\### escaped" */
    fakeBlock->blockString = "\\### escaped";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("\\### escaped", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "\\### escaped");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tLeading and trailing spaces are discarded.\n\n");
    
    /* TEST: "#                  foo                     " */
    fakeBlock->blockString = "foo";
    fakeBlock->blockType   = ATX_HEADING_1;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("#                  foo                     ", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "#                  foo                     ");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tOne to three spaces of indentation is allowed, four or more is too much.\n\n");
    
    /* TEST: " ### foo" */
    fakeBlock->blockString = "foo";
    fakeBlock->blockType   = ATX_HEADING_3;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing(" ### foo", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], " ### foo");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "  ### foo" */
    fakeBlock->blockString = "foo";
    fakeBlock->blockType   = ATX_HEADING_3;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("  ### foo", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "  ### foo");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "   ### foo" */
    fakeBlock->blockString = "foo";
    fakeBlock->blockType   = ATX_HEADING_3;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("   ### foo", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "   ### foo");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "    ### foo" */
    fakeBlock->blockString = "### foo";
    fakeBlock->blockType   = INDENTED_CODE_BLOCK;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("    ### foo", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[9], "    ### foo");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tA closing sequence of %s#%s characters is optional. If included, they\n", bold, reset);
    printf("\tneed not be the same length as the opening sequence.\n\n");
    
    /* TEST: "## foo ##" */
    fakeBlock->blockString = "foo";
    fakeBlock->blockType   = ATX_HEADING_2;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("## foo ##", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[4], "## foo ##");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "   ### foo ###" */
    fakeBlock->blockString = "foo";
    fakeBlock->blockType   = ATX_HEADING_3;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("   ### foo ###", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[5], "   ### foo ###");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "# foo ##################################" */
    fakeBlock->blockString = "foo";
    fakeBlock->blockType   = ATX_HEADING_1;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("# foo ##################################", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[3], "# foo ##################################");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tSpaces are allowed after the optional closing sequence, just not other\n");
    printf("\tnon-whitespace characters.\n\n");
    
    /* TEST: "### foo ###     " */
    fakeBlock->blockString = "foo";
    fakeBlock->blockType   = ATX_HEADING_3;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("### foo ###     ", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[5], "### foo ###     ");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "### foo ### b" */
    fakeBlock->blockString = "foo ### b";
    fakeBlock->blockType   = ATX_HEADING_3;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("### foo ### b", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[5], "### foo ### b");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tThe closing sequence must be preceded by a space.\n\n");
    
    /* TEST: "### foo#" */
    fakeBlock->blockString = "foo#";
    fakeBlock->blockType   = ATX_HEADING_3;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("### foo#", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[5], "### foo#");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tEscaped %s#%s characters do not count as part of the closing sequence\n\n", bold, reset);
    
    /* TEST: "## foo #\##" */
    fakeBlock->blockString = "foo ###";
    fakeBlock->blockType   = ATX_HEADING_2;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("## foo #\\##", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[4], "## foo #\\##");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tATX Headings do not need to be separated from surrounding content by \n");
    printf("\tblank lines, they can even interrupt paragraphs.\n\n");
    
    /* TEST: "## foo" */
    fakeBlock->blockString = "foo";
    fakeBlock->blockType   = ATX_HEADING_2;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = PARAGRAPH;
    returnValue   = testBlockParsing("## foo", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, "(PARAGRAPH)", "## foo");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "## foo" */
    fakeBlock->blockString = "foo";
    fakeBlock->blockType   = ATX_HEADING_2;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = HORIZONTAL_RULE;
    returnValue   = testBlockParsing("## foo", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, "(HORIZONTAL_RULE)", "## foo");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tATX Headings can be empty.\n\n");
    
    /* TEST: "# " */
    fakeBlock->blockString = "";
    fakeBlock->blockType   = ATX_HEADING_1;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("# ", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[3], "# ");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "# #" */
    fakeBlock->blockString = " ";
    fakeBlock->blockType   = ATX_HEADING_1;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("# #", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[3], "# #");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "###" */
    fakeBlock->blockString = "";
    fakeBlock->blockType   = ATX_HEADING_3;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("###", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[5], "###");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
   /* ==============================================================
    * == SETEXT HEADINGS
    * =========================================================== */
    
    printf("\t%sSETEXT HEADINGS:%s\n\n", bold, reset);
    
    printf("\tA setext heading consists of one or more lines of text, each containing\n");
    printf("\tat least one non-whitespace character, followed by a setext heading\n");
    printf("\tunderline. Setext headings are basically paragaphs in which the last\n");
    printf("\tline before the BLANK_LINE is a line of %s=%s\'s or %s-%s\'s.\n\n", bold, reset, bold, reset);
    
    /* TEST: "Foo bar\n======" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = SETEXT_HEADING_1;
    fakeBlock->insertType  = UPDATE_TYPE;
    lastBlockType = PARAGRAPH;
    returnValue   = testBlockParsing("======", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[11], "Foo bar\\n======");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "Foo bar\n------" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = SETEXT_HEADING_2;
    fakeBlock->insertType  = UPDATE_TYPE;
    lastBlockType = PARAGRAPH;
    returnValue   = testBlockParsing("------", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[12], "Foo bar\\n------");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tThe underlining can be of any length.\n\n");
    
    /* TEST: "Foo bar\n-------------------" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = SETEXT_HEADING_2;
    fakeBlock->insertType  = UPDATE_TYPE;
    lastBlockType = PARAGRAPH;
    returnValue   = testBlockParsing("-------------------", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[12], "Foo bar\\n-------------------");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "Foo bar\n=" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = SETEXT_HEADING_1;
    fakeBlock->insertType  = UPDATE_TYPE;
    lastBlockType = PARAGRAPH;
    returnValue   = testBlockParsing("=", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[11], "Foo bar\\n=");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tThe heading content can be indented up to three spaces, and need not\n");
    printf("\twith the underlining. Four spaces is an INDENTED_CODE_BLOCK.\n\n");
    
    /* TEST: "    Foo bar\n    =" */
    fakeBlock->blockString = "=";
    fakeBlock->blockType   = INDENTED_CODE_BLOCK;
    fakeBlock->insertType  = APPEND_STRING;
    lastBlockType = INDENTED_CODE_BLOCK;
    returnValue   = testBlockParsing("    =", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[9], "    foo\\n    =");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tThe heading underline cannot contain spaces or other characters.\n\n");
    
    /* TEST: "Foo bar\n= = = = =" */
    fakeBlock->blockString = "= = = = =";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = APPEND_STRING;
    lastBlockType = PARAGRAPH;
    returnValue   = testBlockParsing("= = = = =", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "Foo bar\\n= = = = =");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "Foo bar\n====a" */
    fakeBlock->blockString = "====a";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = APPEND_STRING;
    lastBlockType = PARAGRAPH;
    returnValue   = testBlockParsing("====a", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "Foo bar\\n====a");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    printf("\tSetext heading underlines must follow a previously parsed paragraph\n");
    printf("\tor, setext headings cannot be empty.\n\n");
    
    /* TEST: "\n====" */
    fakeBlock->blockString = "====";
    fakeBlock->blockType   = PARAGRAPH;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("====", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[10], "====");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);
    
    /* TEST: "\n---" */
    fakeBlock->blockString = "NULL";
    fakeBlock->blockType   = HORIZONTAL_RULE;
    fakeBlock->insertType  = INSERT_NODE;
    lastBlockType = BLANK_LINE;
    returnValue   = testBlockParsing("----", &fakeBlock, lastBlockType);
    printf("\t%s %s%s: \'%s\'\n", returnValue ? pass : fail, test, returnTypes[3], "----");
    printf("\t\tReturn value: \'%s\'\n", fakeBlock->blockString);
    printf("\t\tReturn type: %s\n\n", returnTypes[fakeBlock->blockType]);

    return 0;
}


/* Send the input *string to the parseBlockType() function in parse.c,
 * compare the temp_block_node_t that is returned with the one that 
 * was sent to the function as an argument. If there are the same,
 * return true; otherwise, assign all the members of the returned
 * temp_block_node_t to the pointer that was passed as an argument.
 * Then return false.
 */
static int testBlockParsing(const char *string, temp_block_node_t **fakeBlock, const mdblock_t lastBlockType)
{
    temp_block_node_t *returnBlock = parseBlockType(string, lastBlockType);
    
    if (strcmp(returnBlock->blockString, (*fakeBlock)->blockString) == 0 &&
        returnBlock->blockType  == (*fakeBlock)->blockType &&
        returnBlock->insertType == (*fakeBlock)->insertType)
    {
        return 1;
    }
    else
    {
        (*fakeBlock)->blockString = returnBlock->blockString;
        (*fakeBlock)->blockType   = returnBlock->blockType;
        (*fakeBlock)->insertType  = returnBlock->insertType;
        return 0;
    }
}


/* Test the parseArguments() function in arguments.c against all the
 * possible VALID command-line arguments. Calling parseArguments()
 * with specific input can cause all other functions in arguments.c
 * to be executed, which is the idea here.
 *
 * NOTE: All INVALID command-line arguments will result in the exit()
 *       function begin called, therefore, it is hard to test them.
 */
static int testArguments(const char *argv[], const char argc, const argtype_t argt[])
{
    // Call parseArguments() with fake input
    argtype_t *returnArgt = parseArguments(argc, argv);
    
    // Compare it to argt to determine if its valid
    for (size_t i = 0; i < argc; ++i)
    {
        if (returnArgt[i] != argt[i])
        {
            return 0;
        }
    }
    
    // Return true, all arguments match
    return 1;
}


/* Test the writeLine() and readLine() functions in files.c. Before
 * either of these calls can be made, a call to openFile() must be 
 * made. The success of failure of these calls is returned (as an
 * integer boolean).
 */
static int testFileOpening(const char fileName[], const argtype_t fileType, const int readwrite)
{
    FILE *fp = openFile(fileName, fileType);
    int returnValue = 1;
    
    if (readwrite == 0) // write to the file
    {
        writeLine(fp, 1, "test string to be written to file");
    }
    else // read from file
    {
        char *string = readLine(fp);
        returnValue = (string == NULL) ? 0 : 1;
    }
    
    fclose(fp);
    return returnValue;
}