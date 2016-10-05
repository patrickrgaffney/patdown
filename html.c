/*****
 * html.c -- html output writer
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-10-04
 * @modified    2016-10-04
 * 
 ************************************************************************/

#include <stdio.h>

#include "html.h"
#include "markdown.h"
#include "strings.h"

static void write_paragraph(FILE *fp, String *p)
{
    fprintf(fp, "<p>%s</p>\n", p->string);
}

static void write_header(FILE *fp, String *h, size_t level)
{
    fprintf(fp, "<h%zu>%s</h%zu>\n", level, h->string, level);
}

static void write_hr(FILE *fp)
{
    fprintf(fp, "<hr>\n");
}

static void write_code(FILE *fp, String *c, char *lang)
{
    /* Do something with lang. */
    fprintf(fp, "<pre><code>%s</code></pre>", c->string);
}

static void write_html(FILE *fp, String *h)
{
    fprintf(fp, "%s", h->string);
}

void output_html(FILE *fp, Markdown *node)
{
    while (node) {
        switch (node->type) {
            case PARAGRAPH: 
                write_paragraph(fp, node->value);
                break;
            case ATX_HEADER_1:
            case SETEXT_HEADER_1:
                write_header(fp, node->value, 1);
                break;
            case ATX_HEADER_2:
            case SETEXT_HEADER_2:
                write_header(fp, node->value, 2);
                break;
            case ATX_HEADER_3:
                write_header(fp, node->value, 3);
                break;
            case ATX_HEADER_4:
                write_header(fp, node->value, 4);
                break;
            case ATX_HEADER_5:
                write_header(fp, node->value, 5);
                break;
            case ATX_HEADER_6:
                write_header(fp, node->value, 6);
                break;
            case HORIZONTAL_RULE:
                write_hr(fp);
                break;
            case INDENTED_CODE_BLOCK:
                write_code(fp, node->value, NULL);
                break;
            case FENCED_CODE_BLOCK:
                write_code(fp, node->value, ((CodeBlock *)node->data)->lang);
                break;
            case HTML_BLOCK:
                write_html(fp, node->value);
                break;
            default: break;
        }
        node = node->next;
    }
}