/*****
 * links.c -- data methods for link manipulation
 * 
 * @author      Pat Gaffney <pat@hypepat.com>
 * @created     2016-09-30
 * @modified    2016-09-30
 *
 ************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "errors.h"
#include "links.h"

/************************************************************************
 * @section Link Reference Methods
 *
 *  These methods create a binary search tree of LinkRef nodes that can
 *  then be searched to resolve inline link references.
 ************************************************************************/

/** Allocate space for new LinkRef node. ********************************/
LinkRef *init_link_ref(void)
{
    LinkRef *ref = NULL;
    ref = malloc(sizeof(LinkRef));
    if (!ref) throw_memory_error();
    
    ref->left  = NULL;
    ref->right = NULL;
    return ref;
}

/** Insert a LinkRef node into the tree. ********************************/
void insert_link_ref(LinkRef **head, LinkRef *node)
{
    if (!*head) *head = node;
    else {
        int rc = strcmp(node->label, (*head)->label);
        if (rc < 0) {
            insert_link_ref(&((*head)->left), node);
        }
        else if (rc > 0) {
            insert_link_ref(&((*head)->right), node);
        }
        else printf("DUPLICATE NODE: \'%s\'\n", node->label);
    }
}

/** Search LinkRef tree for node by link label. *************************/
LinkRef *search_link_refs(LinkRef *head, char *label)
{
    if (!head) return NULL;
    
    int rc = strcmp(label, head->label);
    if (rc < 0) {
        search_link_refs(head->left, label);
    }
    else if (rc > 0) {
        search_link_refs(head->right, label);
    }
    
    return head;
}

/** Debug-print a tree of LinkRef nodes -- in-order traversal ***********/
void print_link_refs(LinkRef *node)
{
    if (node) {
        print_link_refs(node->left);
        printf("[%s]: <%s> -> \'%s\'\n", node->label, node->dest, node->title);
        print_link_refs(node->right);
    }
}

/** Free a LinkRef tree -- starting at node. **/
void free_link_ref_tree(LinkRef *node)
{
    if (node) {
        free_link_ref_tree(node->left);
        free_link_ref_tree(node->right);
        free(node);
    }
}