# `patdown` — a Markdown parser

`patdown` is my personal Markdown parser. It is still very-much a work-in-progress and does not have capabilities for the full Markdown spec.

The aim of the this project is to write a [CommonMark][cm]-compliant parser in C — without using regular expressions. All of the parsing logic is strung out over the logical statements available to C.

**NOTE:** I am not taking a politicized stance on regular expressions or their usefulness. I only wanted an interesting challenge.

[cm]: http://commonmark.org/