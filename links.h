/*****
 * links.h -- data methods for link manipulation
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-09-30
 *  modified:   2016-10-13
 *  project:    patdown
 *
 ************************************************************************/

#pragma once

/************************************************************************
 * Link Reference Data Structures
 ************************************************************************/

/** LinkRef -- node to hold link data information **/
typedef struct LinkRef
{
    char label[1000];       /* Link label: unique identifier for links. */
    char dest[1000];        /* Link destination: the URL to link to. */
    char title[1000];       /* Link title: optional, title attribute. */
    struct LinkRef *left;   /* Pointer to left node in the tree. */
    struct LinkRef *right;  /* Pointer to right node in the tree. */
} LinkRef;


/************************************************************************
 * Link Reference Methods
 *
 *  These methods create a binary search tree of LinkRef nodes that can
 *  then be searched to resolve inline link references.
 ************************************************************************/

/** Allocate space for new LinkRef node. **/
LinkRef *init_link_ref(void);

/** Insert a LinkRef node into the tree. **/
void insert_link_ref(LinkRef **head, LinkRef *node);

/** Search LinkRef tree for node by link label. **/
LinkRef *search_link_refs(LinkRef *head, char *label);

/** Debug-print a tree of LinkRef nodes. **/
void print_link_refs(LinkRef *node);

/** Free a LinkRef tree -- starting at node. **/
void free_link_ref_tree(LinkRef *node);
