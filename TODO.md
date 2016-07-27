### 1. switch to a recursive-descent parser

- at minimum, make the parser top-down.
- `main()` should call `markdown()`
    - then `markdown()` should do all the parsing
    - this is correctly working now.
- instead of passing the open input file to `markdown()`
    - the parser should just call `get_line()` or something..
    - **NOTE:** the *parser* should call this function, not `markdown()`
- i want all the methods and structures for dealing with markdown in `markdown.c`
- i want all the parsing methods in `parsers.c`
    - difference being that `parsers.c` should call `get_line()`
    - this would help immensely with issues like #2 below
    - that way we arent writing hack-y functions like `update_queue()`
        - this functions doesnt really update the queue necessarily
        - it finishes parsing the line returned from the parser
        - but currently this is the only way for this to work
        - because the parser isnt smart enought to know when to get another line to see if it needs appending/updating/etc.


### 2. fix line breaks (`<br>`) inside paragraphs

- fix PARAGRAPH's so that we can later parse for line breaks
    - as of right now, if two lines in a row, it will be impossible to parse line breaks later

```
line1
line2
```

- if `line1` has trailing spaces, we don't get rid of them
    - but we will append `line2` to the end of `line1`

```
line1   line2
```

- perhaps we could add a nonprintable character to act as a placeholder for where the original line break was

```
line1  § line2
```