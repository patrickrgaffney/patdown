# `patdown` — a Markdown parser

`patdown` is a Markdown parser. It is still very-much a work-in-progress and does not implement the full Markdown spec.

The aim of the this project is to write a [CommonMark][cm]-compliant parser in C99 with the following caveats:

-   no regular expressions or peg's  

    Most of the Markdown parsers in use today either uses a peg grammar or a series of multi-line regular expressions. I want something closer to [sundown](https://github.com/vmg/sundown), only with modern Markdown extensions.
    
-   no external dependencies  

    This project should compile with any respectable C99 compiler.

[cm]: http://commonmark.org/