#include <stddef.h>
#include <stdbool.h>
#include "markdown.h"

typedef struct md_code_block_t
{
    char fence; // fence character used
    string_t *info; // info string from opening fence
} md_code_block_t;

typedef struct md_blockquote_t
{
    size_t level; // level of nested blockquote
} md_blockquote_t;

typedef struct md_list_t
{
    bool ordered; // true for <ol>, false for <ul>
    char marker; // marker character used
    size_t indentation; // level of currently nested list
} md_list_t;

typedef struct link_ref_t
{
    string_t *label;
    string_t *url;
    string_t *title;
} link_ref_t;