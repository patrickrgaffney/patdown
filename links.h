/**
 * links.h -- data methods for link manipulation
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-09-30
 *  modified:   2016-12-30
 *  project:    patdown
 *
 ************************************************************************/

#pragma once

/************************************************************************
 * # Link Reference Type
 *
 * A `LinkRef` object is a node in the binary search tree that's 
 * internal to the links.c file. This type is exposed so that links can 
 * be inserted into the `Markdown` queue. This is useful for testing and 
 * semantic analysis of other links / the markdown file itself.
 *
 ************************************************************************/

/**
 * A type to hold link data information. 
 * 
 * There are three important pieces of a link: a label, destination, and
 * title. Each are stored as static arrays of 1000 `char` elements.
 *
 * - member label: the link label -- unique identifier for every link.
 * - member dest: the link destination -- the URL to link to.
 * - member title: the link title -- an optional `title` attribute.
 * - member left: a pointer to the left subtree relative to self.
 * - member right: a pointer to the right subtree relative to self.
 */
typedef struct LinkRef
{
    char label[1000];
    char dest[1000];
    char title[1000];
    struct LinkRef *left;
    struct LinkRef *right;
} LinkRef;


/************************************************************************
 * # Link Reference Methods
 *
 * These methods create a binary search tree of `LinkRef` nodes that can
 * then be searched to resolve inline link references.
 *
 ************************************************************************/

/** Allocate space for new `LinkRef` node. */
LinkRef *init_link_ref(void);

/** Add a `LinkRef` node the the internal binary search tree. */
// void add_link_ref(LinkRef *node);

/** Search the binary tree for a particular link label. */
// LinkRef *search_link_refs(char *label);

/** Free all nodes in the private binary search tree. */
void free_link_refs(void);
