/* 
 * bst.c 
 * Lab5: Binary Search Trees
 * ECE 2230, Fall 2018
 * David Lynge
 * dlynge
 */

// Options for the tree policy
enum balanceoptions {BST, AVL, TWOTHREET};

// Data pointer for a node
typedef void *data_t;

// Key for a node
typedef int bst_key_t;

// Individual node
typedef struct bst_node_tag {
    data_t data_ptr;
    bst_key_t key;
    struct bst_node_tag *left;
    struct bst_node_tag *right;
    struct bst_node_tag *parent;
} bst_node_t;

// Header for tree
typedef struct bst_tag {
    bst_node_t *root;
    int tree_policy;
    int tree_size;
    int num_recent_key_comparisons;
} bst_t;

/* 
 * Finds the tree element with the matching key and returns that
 * node's data pointer. If no key is found, returns NULL.
 */
data_t bst_access(bst_t *, bst_key_t);

/*
 * Creates and initializes the header block for a new tree.
 * Returns the pointer to the newly created tree.
 */
bst_t *bst_construct (int tree_policy);

/*
 * Frees all items in a tree and its header block & data pointer.
 */
void bst_destruct (bst_t *);

/*
 * Inserts a new node with specified data and key. Returns 0 if
 * the data was replaced at a pre-existing key or 1 if a new node
 * was created.
 */
int bst_insert (bst_t *, bst_key_t, data_t);

/*
 * Removes an item from the tree with the matching key. Returns the
 * data pointer and frees the node.
 */
data_t bst_remove (bst_t *, bst_key_t);

/*
 * Returns the number of keys in the BST
 */
int bst_size(bst_t *);

/*
 * Returns the number of number of comparisons for the most
 * recent function call
 */
int bst_stats (bst_t *);

/*
 * Returns the internal path length of the tree
 */
int bst_int_path_len(bst_t *);

/*
 * Prints the contents of the tree
 */
void bst_debug_print_tree(bst_t *);

/*
 * Verifies that the input is a proper tree
 */
void bst_debug_validate(bst_t *);
