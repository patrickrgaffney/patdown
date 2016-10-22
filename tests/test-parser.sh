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
BINARY="$(dirname $PWD)/$PROG"

## Colors ##
GREEN=$'\e[32m'
BOLD=$'\e[1m'
RED=$'\e[31m'
RESET=$'\e[0m'

echo "$BOLD Run the parser tests for $BOLD$PROG$RESET:"

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
            if [[ $answer == $result ]]; then
                echo -e "$BOLD$GREEN --> 🍺  PASSED: $testfile $RESET"
            else
                echo -e "$BOLD$RED --> 🖕🏽  FAILED: $testfile $RESET"
                echo -e "\t patdown output:\n"
                echo -e "$result"
                echo -e "\t assertion:\n"
                echo -e "$answer"
            fi
        else
            echo -e "$BOLD$RED --> 📁  FAILED: $outfile does not exist!"
        fi
    fi
done