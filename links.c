/**
 * links.c -- binary search tree of links
 * 
 *  author:     Pat Gaffney <pat@hypepat.com>
 *  created:    2016-09-30
 *  modified:   2016-12-30
 *  project:    patdown
 *
 ************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "errors.h"
#include "links.h"


/************************************************************************
 * Link Reference Binary Search Tree
 *
 * A private binary search of `LinkRef` nodes is created as the input
 * file is parsed. This tree is then queried as the actual references
 * are encountered in the file.
 *
 ************************************************************************/

/** Binary Search Tree Pointer. */
static LinkRef *head = NULL;


/** 
 * Allocate space for new `LinkRef` node.
 *
 * - throws fatal_memory_error: Memory could not be allocated.
 *
 * - returns: A pointer to the new `LinkRef` node.
 */
LinkRef *init_link_ref(void)
{
    LinkRef *ref = (LinkRef *) malloc(sizeof(LinkRef));
    if (!ref) throw_fatal_memory_error();
    
    /* A link title is optional. */
    ref->title[0] = '\0';
    
    ref->left  = NULL;
    ref->right = NULL;
    return ref;
}


/**
 * Insert a `LinkRef` node into the binary tree.
 *
 * - TODO: Implement duplicate detection (should overwrite).
 *
 * - parameter node: The node to insert into the tree.
 */
// static void insert_link_ref(LinkRef *top, LinkRef *node)
// {
//     if (!node) return;
//     if (!top) top = node;
//     else {
//         int rc = strcmp(node->label, top->label);
//         if (rc < 0) {
//             insert_link_ref(top->left, node);
//         }
//         else if (rc > 0) {
//             insert_link_ref(top->right, node);
//         }
//         else printf("DUPLICATE NODE: \'%s\'\n", node->label);
//     }
// }


/**
 * Add a `LinkRef` node the the internal binary search tree.
 *
 * This function is the public API for inserting nodes.
 *
 * - parameter node: The node to insert into the tree.
 */
// void add_link_ref(LinkRef *node)
// {
//     insert_link_ref(head, node);
// }


/**
 * Search the binary tree for a particular link label.
 *
 * - parameter label: The link to search for.
 *
 * - returns: A pointer to the link, if found, otherwise `NULL`.
 */
// LinkRef *search_link_refs(LinkRef *top, char *label)
// {
//     if (!head) return NULL;
//
//     int rc = strcmp(label, head->label);
//     if (rc < 0) {
//         return search_link_refs(head->left, label);
//     }
//     else if (rc > 0) {
//         return search_link_refs(head->right, label);
//     }
//     return head;
// }


/** 
 * Free the memory allocated for a `LinkRef` node.
 *
 * This is the internal interface for freeing a particular `LinkRef` node. 
 * All nodes below the parameter in the tree will also be free'd.
 *
 * - parameter node: The node at which to begin freeing.
 */
static void free_link_ref_node(LinkRef *node)
{
    if (node) {
        free_link_ref_node(node->left);
        free_link_ref_node(node->right);
        free(node);
    }
}


/**
 * Free all nodes in the private binary search tree.
 *
 * This is the external interface for freeing the internal
 * binary serach tree created by parsing the input file.
 */
void free_link_refs(void)
{
    free_link_ref_node(head);
}
