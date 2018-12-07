/* 
 * bst.c 
 * Lab5: Binary Search Trees
 * ECE 2230, Fall 2018
 * David Lynge
 * dlynge
 * 
 * This file contains all the functions necessary to track and maintain a 
 * binary search tree (bst).
 * 
 * Assumptions: We will assume that only non-negative keys are inserted into
 * the list. Also, we assume that these functions are not given a NULL pointer
 * to operate on when asking for a tree pointer.
 * 
 * Bugs: none known
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>

#include "bst.h"


#define TRUE 1
#define FALSE 0

// ----- Private Functions -----

/*
 * Finds a specific node with a key value. Returns the found node or NULL if
 * no node exists with that key.
 */
bst_node_t *bst_find(bst_t *tree, bst_key_t key)
{
    bst_node_t *rover = tree->root;

    tree->num_recent_key_comparisons = 0;

    while (rover != NULL)
        if (tree->num_recent_key_comparisons++, key == rover->key)
            return rover;
        else if (tree->num_recent_key_comparisons++, key > rover->key)
            rover = rover->right;
        else
            rover = rover->left;
    
    return NULL;
}

/*
 * Recursively accesses the bst, removing each node along the way.
 */
void bst_destruct_recursive (bst_node_t *node)
{
    if (node == NULL) return;

    bst_destruct_recursive(node->left);
    bst_destruct_recursive(node->right);

    free(node->data_ptr);
    free(node);
}

/*
 * Recursively prints the bst, with the root on the left and leafs on the right
 */
void ugly_print(bst_node_t *N, int level) 
{
    int i;
    if (N == NULL) return;

    ugly_print(N->right, level+1);
    
    for (i=0; i<level; i++) printf("     ");
    printf("%5d\n", N->key);
    ugly_print(N->left, level+1);
}

/*
 * Recursively goes through the bst, making sure that all the nodes are in order 
 * and the pointers are correct. Returns TRUE if everything seems okay, and
 * returns FALSE if the bst has a problem.
 */
int bst_debug_validate_rec(bst_node_t *N, int min, int max, int *count)
{
    if (N == NULL) return TRUE;
    if (N->key <= min || N->key >= max) 
        return FALSE;
    assert(N->data_ptr != NULL);
    if (N->left != NULL && N->left->parent != N)
        return FALSE;
    if (N->right != NULL && N->right->parent != N)
        return FALSE;
    *count += 1;        
    return bst_debug_validate_rec(N->left, min, N->key, count) &&
    bst_debug_validate_rec(N->right, N->key, max, count);
}

/*
 * Recursively goes through the bst and adds up the interior path length. Returns
 * the path length for a node plus the path length for its children. Returns 0
 * if the input node is a NULL pointer.
 */
int bst_path_recursive(bst_node_t *node)
{
    int len = 0;
    bst_node_t *rover = node;

    if (node == NULL)
        return 0;

    while (rover->parent != NULL)
    {
        rover = rover->parent;
        len++;
    }
    return len + bst_path_recursive(node->right) + bst_path_recursive(node->left);
}

// ----- Public Functions -----

/* 
 * Finds the tree element with the matching key and returns that
 * node's data pointer. If no key is found, returns NULL.
 */
data_t bst_access(bst_t *tree, bst_key_t key)
{
    bst_node_t *node;

    if (tree == NULL || tree->root == NULL) return NULL;

    node = bst_find(tree, key);

    if (node == NULL)
        return NULL;
    else
        return node->data_ptr;
}

/*
 * Creates and initializes the header block for a new tree.
 * Returns the pointer to the newly created tree.
 */
bst_t *bst_construct (int tree_policy)
{
    bst_t *header = malloc(sizeof(bst_t));
    header->root = NULL;
    header->tree_policy = BST;
    header->tree_size = 0;
    header->num_recent_key_comparisons = 0;

    return header;
}

/*
 * Frees all items in a tree and its header block & data pointer.
 */
void bst_destruct (bst_t *tree)
{
    assert(tree != NULL);
    
    bst_destruct_recursive(tree->root);
    free(tree);
}

/*
 * Inserts a new node with specified data and key. Returns 0 if
 * the data was replaced at a pre-existing key or 1 if a new node
 * was created.
 */
int bst_insert (bst_t *tree, bst_key_t key, data_t data)
{
    assert(tree != NULL);

    bst_node_t *newNode;
    bst_node_t *rover = tree->root;
    bst_node_t *follower = rover;

    tree->num_recent_key_comparisons = 0;

    while (rover != NULL)
    {
        follower = rover;

        if (tree->num_recent_key_comparisons++, key == rover->key)
        {
            free(rover->data_ptr);
            rover->data_ptr = data;
            return 0;
        }
        else if (tree->num_recent_key_comparisons++, key > rover->key)
            rover = rover->right;
        else
            rover = rover->left;
    }

    newNode = malloc(sizeof(bst_node_t));
    newNode->data_ptr = data;
    newNode->key = key;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = follower;

    if (follower == NULL)
        tree->root = newNode;
    else
        if (key > follower->key)
            follower->right = newNode;
        else
            follower->left = newNode;
    
    tree->tree_size++;
    
    return 1;
}

/*
 * Removes an item from the tree with the matching key. Returns the
 * data pointer and frees the node.
 */
data_t bst_remove (bst_t *tree, bst_key_t key)
{
    assert(tree != NULL);
    
    bst_node_t *node = bst_find(tree, key);
    bst_node_t *successor;
    data_t ret;

    //bst_debug_print_tree(tree);

    if (node == NULL) return NULL;

    // node has no children
    if (node->right == NULL && node->left == NULL)
        // node is root
        if (node == tree->root)
            tree->root = NULL;
        // node is not root
        else if (key > node->parent->key)
            node->parent->right = NULL;
        else
            node->parent->left = NULL; 
    // node has only right child
    else if (node->right != NULL && node->left == NULL)
    {
        if (node == tree->root)
            tree->root = node->right;
        else if (key > node->parent->key)
            node->parent->right = node->right;
        else
            node->parent->left = node->right;
        
        node->right->parent = node->parent;
    }
    // node has only left child
    else if (node->right == NULL && node->left != NULL)
    {
        if (node == tree->root)
            tree->root = node->left;
        else if (key > node->parent->key)
            node->parent->right = node->left;
        else
            node->parent->left = node->left;
        
        node->left->parent = node->parent;
    }
    // node has both children
    else
    {
        // find successor
        successor = node->right;
        while (successor->left != NULL) successor = successor->left;
        
        // Move node below successor
        if (successor->right != NULL)
        {
            successor->right->parent = successor->parent;
            if (successor->parent == node)
                node->right = successor->right;
            else
                successor->parent->left = successor->right;
        }
        else
            if (successor->parent == node)
                node->right = NULL;
            else
                successor->parent->left = NULL;

        // Move successor node
        successor->left = node->left;
        successor->right = node->right;
        successor->parent = node->parent;
        if (successor->left != NULL)
            successor->left->parent = successor;
        if (successor->right != NULL)
            successor->right->parent = successor;

        if (node == tree->root)
            tree->root = successor;
        else if (key > node->parent->key)
            node->parent->right = successor;
        else
            node->parent->left = successor;
    }
    
    // if (successor->left == successor || successor->right == successor)
    //     printf("Somethin' ain't right boss\n");
    
    tree->tree_size--;
    ret = node->data_ptr;
    free(node);
    return ret;
}

/*
 * Returns the number of keys in the BST
 */
int bst_size(bst_t *tree)
{
    return tree->tree_size;
}

/*
 * Returns the number of number of comparisons for the most
 * recent function call
 */
int bst_stats (bst_t *tree)
{
    return tree->num_recent_key_comparisons;
}

/*
 * Returns the internal path length of the tree
 */
int bst_int_path_len(bst_t *tree)
{
    return bst_path_recursive(tree->root);
}

/*
 * Prints the contents of the tree
 */
void bst_debug_print_tree(bst_t *tree)
{
    ugly_print(tree->root, 0);
}

/*
 * Verifies that the input is a proper tree
 */
void bst_debug_validate(bst_t *tree)
{
    int size = 0;
    assert(bst_debug_validate_rec(tree->root, INT_MIN, INT_MAX, &size) == TRUE);
    assert(size == tree->tree_size);
}
