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
#include <string.h>

#include "arguments.h"
#include "files.h"

/* Static function definitions */
static int testArguments(const char *argv[], const char argc, const ArgType argt[]);
static int testFileOpening(const char fileName[], const ArgType fileType);


int main(int argc, char const *argv[])
{
    /* Every test prints a string with the following format:
     *      _ TEST: string
     * Where `_` is either a '√' for PASS or a '✕' for FAIL and
     * `string` is a description of the test. If the test is PASSed,
     * the color of `_ TEST:` is changed to green. If the test is 
     * FAILed, the color is changed to red. Both color changes are 
     * set using ANSI color codes.
     */
    const char pass[]  = "\x1b[32m√";
    const char fail[]  = "\x1b[31m✕";
    const char test[]  = "TEST: \x1b[0m";
    const char blue[]  = "\x1b[34m";
    const char reset[] = "\x1b[0m";
    
    /* ==============================================================
     * == COMMAND-LINE ARGUMENTS
     * =========================================================== */
    
    printf("%sCommand-line arguments:%s\n", blue, reset);
    
    // patdown --help
    const char *fakeArgv[] = {"patdown", "--help"};
    ArgType fakeArgt[] = {PROGRAM_NAME, HELP_FLAG};
    const char string[] = "patdown --help";
    printf("\t%s %s\'%s\'\n", testArguments(fakeArgv, 2, fakeArgt) ? pass : fail, test, string);

    // patdown --version
    const char *fakeArgv2[] = {"patdown", "--version"};
    ArgType fakeArgt2[] = {PROGRAM_NAME, VERSION_FLAG};
    const char string2[] = "patdown --version";
    printf("\t%s %s\'%s\'\n", testArguments(fakeArgv2, 2, fakeArgt2) ? pass : fail, test, string2);
         
    // patdown --help inFile
    const char *fakeArgv3[] = {"patdown", "--help", "inFile"};
    ArgType fakeArgt3[] = {PROGRAM_NAME, HELP_FLAG, INPUT_FILE_NAME};
    const char string3[] = "patdown --help inFile";
    printf("\t%s %s\'%s\'\n", testArguments(fakeArgv3, 3, fakeArgt3) ? pass : fail, test, string3);
    
    // patdown --version inFile
    const char *fakeArgv4[] = {"patdown", "--version", "inFile"};
    ArgType fakeArgt4[] = {PROGRAM_NAME, VERSION_FLAG, INPUT_FILE_NAME};
    const char string4[] = "patdown --version inFile";
    printf("\t%s %s\'%s\'\n", testArguments(fakeArgv4, 3, fakeArgt4) ? pass : fail, test, string4);
    
    // patdown --version --help inFile
    const char *fakeArgv5[] = {"patdown", "--version", "--help", "inFile"};
    ArgType fakeArgt5[] = {PROGRAM_NAME, VERSION_FLAG, HELP_FLAG, INPUT_FILE_NAME};
    const char string5[] = "patdown --version --help inFile";
    printf("\t%s %s\'%s\'\n", testArguments(fakeArgv5, 4, fakeArgt5) ? pass : fail, test, string5);
    
    // patdown --version inFile --help
    const char *fakeArgv6[] = {"patdown", "--version", "inFile", "--help"};
    ArgType fakeArgt6[] = {PROGRAM_NAME, VERSION_FLAG, INPUT_FILE_NAME, HELP_FLAG};
    const char string6[] = "patdown --version inFile --help";
    printf("\t%s %s\'%s\'\n", testArguments(fakeArgv6, 4, fakeArgt6) ? pass : fail, test, string6);
    
    // patdown --version inFile --help -o outFile
    const char *fakeArgv7[] = {"patdown", "--version", "inFile", "--help", "-o", "outFile"};
    ArgType fakeArgt7[] = {PROGRAM_NAME, VERSION_FLAG, INPUT_FILE_NAME, HELP_FLAG, OUTPUT_FILE_FLAG, OUTPUT_FILE_NAME};
    const char string7[] = "patdown --version inFile --help -o outFile";
    printf("\t%s %s\'%s\'\n", testArguments(fakeArgv7, 6, fakeArgt7) ? pass : fail, test, string7);
    
    // patdown --version inFile --help -f format
    const char *fakeArgv8[] = {"patdown", "--version", "inFile", "--help", "-f", "html"};
    ArgType fakeArgt8[] = {PROGRAM_NAME, VERSION_FLAG, INPUT_FILE_NAME, HELP_FLAG, OUTPUT_FORMAT_FLAG, OUTPUT_FORMAT_NAME};
    const char string8[] = "patdown --version inFile --help -f html";
    printf("\t%s %s\'%s\'\n", testArguments(fakeArgv8, 6, fakeArgt8) ? pass : fail, test, string8);
    
    // patdown -f format inFile
    const char *fakeArgv9[] = {"patdown", "-f", "HTML", "inFile"};
    ArgType fakeArgt9[] = {PROGRAM_NAME, OUTPUT_FORMAT_FLAG, OUTPUT_FORMAT_NAME, INPUT_FILE_NAME};
    const char string9[] = "patdown -f HTML inFile";
    printf("\t%s %s\'%s\'\n", testArguments(fakeArgv9, 4, fakeArgt9) ? pass : fail, test, string9);
    
    // patdown -f format --help
    const char *fakeArgv10[] = {"patdown", "-f", "txt", "--help"};
    ArgType fakeArgt10[] = {PROGRAM_NAME, OUTPUT_FORMAT_FLAG, OUTPUT_FORMAT_NAME, HELP_FLAG};
    const char string10[] = "patdown -f txt --help";
    printf("\t%s %s\'%s\'\n", testArguments(fakeArgv10, 4, fakeArgt10) ? pass : fail, test, string10);
    
    // patdown --help inFile -f format --help -o outFile
    const char *fakeArgv11[] = {"patdown", "inFile", "-f", "TXT", "--help", "-o", "outFile"};
    ArgType fakeArgt11[] = {
        PROGRAM_NAME, INPUT_FILE_NAME, OUTPUT_FORMAT_FLAG, OUTPUT_FORMAT_NAME, 
        HELP_FLAG, OUTPUT_FILE_FLAG, OUTPUT_FILE_NAME
    };
    const char string11[] = "patdown --help inFile -f TXT --help -o outFile";
    printf("\t%s %s\'%s\'\n", testArguments(fakeArgv11, 4, fakeArgt11) ? pass : fail, test, string11);
     
    /* ==============================================================
     * == FILE PROCESSING
     * =========================================================== */
     
    printf("\n");
    printf("%sFile-Processing:%s\n", blue, reset);
    
    // open file for reading
    const char string13[] = "open output file \'test.md\' for writing";
    printf("\t%s %s%s\n", testFileOpening("test.md", OUTPUT_FILE_NAME) ? pass : fail, test, string13);
    
    // open file for reading
    const char string12[] = "open input file \'test.md\' for reading";
    printf("\t%s %s%s\n", testFileOpening("test.md", INPUT_FILE_NAME) ? pass : fail, test, string12);
    
    return 0;
}


/* Test the parseArguments() function in arguments.c against all the
 * possible VALID command-line arguments. Calling parseArguments()
 * with specific input can cause all other functions in arguments.c
 * to be executed, which is the idea here.
 *
 * NOTE: All INVALID command-line arguments will result in the exit()
 *       function begin called, therefore, it is hard to test them.
 */
static int testArguments(const char *argv[], const char argc, const ArgType argt[])
{
    // Call parseArguments() with fake input
    ArgType *returnArgt = parseArguments(argc, argv);
    
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


/* Test the openFile() function in files.c for INPUT_FILE_NAME and 
 * OUTPUT_FILE_NAME ArgTypes's.
 */
static int testFileOpening(const char fileName[], const ArgType fileType)
{
    FILE *filePtr = openFile(fileName, fileType);
    
    if (filePtr == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}