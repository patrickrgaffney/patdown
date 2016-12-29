#####
# test-parser.sh -- run tests against the parser
#
#  author:     Pat Gaffney <pat@hypepat.com>
#  created:    2016-10-21
#  modified:   2016-10-21
#  project:    patdown
#
#   This file runs all of the tests in tests/parser. The input files
#   have the `.md` extension, and their respective output files have the
#   `.out` extension. Tests are done to ensure the resulting string have
#   equality.
#
#   patdown generates parser output for these types -- not HTML.
#
#########################################################################

#!/usr/bin/env bash

## Constants ##
PROG=$'patdown'
DIR="parser"
BASEDIR="$(dirname $PWD)"
TESTDIR="tests"
BINARY="$BASEDIR/$PROG"
MAKECMD="cd $BASEDIR; make; cd $BASEDIR/$TESTDIR"

## Colors ##
GREEN=$'\e[32m'
BOLD=$'\e[1m'
RED=$'\e[31m'
RESET=$'\e[0m'

## Totals ##
PASSED=0
FAILED=0
MISSING=0

# Build the patdown executable if it doesn't exist.
if [ -x "$BINARY" ]; then
    echo -e "$BOLD Run the parser tests for $BOLD$PROG$RESET:\n"
else 
    echo -e "$BOLD$RED ERROR:$RESET$BOLD No $PROG executable found"
    echo -e " No worries though.. I'll cook something up...$RESET\n"
    eval $MAKECMD
    echo -e
    echo -e "$BOLD Run the parser tests for $BOLD$PROG$RESET:\n"
fi

# Test every file in $DIR with the .md extension.
for testfile in "$DIR"/*.md
do
    if [ -f "$testfile" ]; then
        
        # The output files have .out extension -- .md must be removed.
        outfile="${testfile%%.*}.out"
        
        if [ -f "$outfile" ]; then
            
            # Get the output file contents and the output from patdown.
            answer=$(< $outfile)
            result=$(eval $BINARY $testfile)
            
            # Test their respective equality.
            if [[ $answer == "$result" ]]; then
                echo -e "$BOLD$GREEN --> 🍺  PASSED: $testfile $RESET"
                let PASSED++
            else
                echo -e "$BOLD$RED --> 🖕🏽  FAILED: $testfile $RESET\n"
                echo -e "$BOLD input:$RESET"
                echo -e "$(< $testfile)"
                echo -e "$BOLD patdown output:$RESET"
                echo -e "$result"
                echo -e "$BOLD assertion:$RESET"
                echo -e "$answer\n"
                let FAILED++
            fi
        else
            echo -e "$BOLD$RED --> 📁  FAILED: $outfile does not exist!"
            let MISSING++
        fi
    fi
done

echo -e 
echo -e " 🍺 $BOLD$GREEN $PASSED tests $RESET"
echo -e " 🖕🏽 $BOLD$RED $FAILED tests $RESET"
echo -e " 📁 $BOLD $MISSING tests $RESET"