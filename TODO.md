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