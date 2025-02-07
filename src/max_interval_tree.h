#ifndef MAX_INTERVAL_TREE_H
#define MAX_INTERVAL_TREE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#endif // MAX_INTERVAL_TREE_H

#ifndef MAX_INTERVAL_TREE_NAME
#error "Must define MAX_INTERVAL_TREE_NAME"
#endif

#ifndef MAX_INTERVAL_TREE_KEY_TYPE
#error "Must define MAX_INTERVAL_TREE_TYPE"
#endif

#ifndef MAX_INTERVAL_TREE_NEGATIVE_INFINITY
#error "Must define MAX_INTERVAL_TREE_NEGATIVE_INFINITY of the same type as key"
#endif

#ifndef MAX_INTERVAL_TREE_SUM_TYPE
#error "Must define WEIGHTED_TREE_SUM_INTERVAL_SUM_TYPE"
#endif

#ifndef MAX_INTERVAL_TREE_MAX_HEIGHT
#define MAX_INTERVAL_TREE_MAX_HEIGHT 128
#endif

#define MAX_INTERVAL_TREE_CONCAT_(a, b) a ## b
#define MAX_INTERVAL_TREE_CONCAT(a, b) MAX_INTERVAL_TREE_CONCAT_(a, b)
#define MAX_INTERVAL_TREE_TYPED(name) MAX_INTERVAL_TREE_CONCAT(MAX_INTERVAL_TREE_NAME, _##name)
#define MAX_INTERVAL_TREE_FUNC(func) MAX_INTERVAL_TREE_CONCAT(MAX_INTERVAL_TREE_NAME, _##func)

#ifndef MAX_INTERVAL_TREE_LESS_THAN
#define MAX_INTERVAL_TREE_LESS_THAN(a, b) ((a) < (b))
#endif

#ifndef MAX_INTERVAL_TREE_EQUALS
#define MAX_INTERVAL_TREE_EQUALS(a, b) ((a) == (b))
#endif


#define BST_NAME MAX_INTERVAL_TREE_NAME
#define BST_KEY_TYPE MAX_INTERVAL_TREE_KEY_TYPE
#define BST_NODE_EXTRA \
    MAX_INTERVAL_TREE_SUM_TYPE partial_sum; \
    MAX_INTERVAL_TREE_SUM_TYPE summand; \
    uint8_t height;

#ifdef MAX_INTERVAL_TREE_KEY_EQUALS
#define BST_KEY_EQUALS MAX_INTERVAL_TREE_KEY_EQUALS
#endif

#ifdef MAX_INTERVAL_TREE_KEY_LESS_THAN
#define BST_KEY_LESS_THAN MAX_INTERVAL_TREE_KEY_LESS_THAN
#endif

#include "binary_tree/binary_tree.h"

#undef BST_NAME
#undef BST_KEY_TYPE
#undef BST_NODE_EXTRA
#ifndef MAX_INTERVAL_TREE_KEY_EQUALS
#define MAX_INTERVAL_TREE_KEY_EQUALS MAX_INTERVAL_TREE_TYPED(key_equals)
#endif
#ifndef MAX_INTERVAL_TREE_KEY_LESS_THAN
#define MAX_INTERVAL_TREE_KEY_LESS_THAN MAX_INTERVAL_TREE_TYPED(key_less_than)
#endif

#define MAX_INTERVAL_TREE_NODE MAX_INTERVAL_TREE_TYPED(node_t)

#define MAX_INTERVAL_TREE_NODE_MEMORY_POOL_NAME MAX_INTERVAL_TREE_TYPED(node_memory_pool)

#define MEMORY_POOL_NAME MAX_INTERVAL_TREE_NODE_MEMORY_POOL_NAME
#define MEMORY_POOL_TYPE MAX_INTERVAL_TREE_NODE
#include "memory_pool/memory_pool.h"
#undef MEMORY_POOL_NAME
#undef MEMORY_POOL_TYPE

#define MAX_INTERVAL_TREE_NODE_MEMORY_POOL_FUNC(name) MAX_INTERVAL_TREE_CONCAT(MAX_INTERVAL_TREE_NODE_MEMORY_POOL_NAME, _##name)


typedef struct MAX_INTERVAL_TREE_NAME {
    MAX_INTERVAL_TREE_TYPED(node_t) *root;
    MAX_INTERVAL_TREE_TYPED(node_memory_pool) *pool;
} MAX_INTERVAL_TREE_NAME;


void MAX_INTERVAL_TREE_FUNC(push_down_sum_rotate_left)(MAX_INTERVAL_TREE_NODE *node) {
    // push down summand from node
    MAX_INTERVAL_TREE_SUM_TYPE tmp1 = node->summand;
    node->summand = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    node->partial_sum += tmp1;
    MAX_INTERVAL_TREE_SUM_TYPE tmp2 = node->right->summand;
    node->right->summand = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    node->left->summand += tmp1;
    node->right->left->summand += tmp1 + tmp2;
    node->right->right->summand += tmp1 + tmp2;

    node->left->summand += node->summand;
    node->right->summand += node->summand;
    node->summand = (MAX_INTERVAL_TREE_SUM_TYPE)0;

    // perform normal rotation
    MAX_INTERVAL_TREE_FUNC(rotate_left)(node);

    tmp1 = node->left->left->summand + node->left->left->partial_sum;
    tmp2 = node->left->right->summand + node->left->right->partial_sum;
    node->left->partial_sum = (tmp1 > tmp2) ? tmp1 : tmp2;
}

void MAX_INTERVAL_TREE_FUNC(push_down_sum_rotate_right)(MAX_INTERVAL_TREE_NODE *node) {
    MAX_INTERVAL_TREE_SUM_TYPE tmp1 = node->summand;
    node->summand = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    node->partial_sum += tmp1;
    MAX_INTERVAL_TREE_SUM_TYPE tmp2 = node->left->summand;
    node->left->summand = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    node->right->summand += tmp1;
    node->left->right->summand += tmp1 + tmp2;
    node->left->left->summand += tmp1 + tmp2;

    // perform normal rotation
    MAX_INTERVAL_TREE_FUNC(rotate_right)(node);
    tmp1 = node->right->right->summand + node->right->right->partial_sum;
    tmp2 = node->right->left->summand + node->right->left->partial_sum;
    node->right->partial_sum = (tmp1 > tmp2) ? tmp1 : tmp2;
}

bool MAX_INTERVAL_TREE_FUNC(rebalance)(MAX_INTERVAL_TREE_TYPED(stack_t) *stack) {
    MAX_INTERVAL_TREE_NODE *tmp_node;
    bool done = false;
    // rebalance
    while (!MAX_INTERVAL_TREE_FUNC(stack_empty)(stack) && !done) {
        uint8_t tmp_height, old_height;
        tmp_node = MAX_INTERVAL_TREE_FUNC(stack_pop)(stack);
        if (MAX_INTERVAL_TREE_FUNC(node_is_leaf)(tmp_node)) {
            // skip if there's a leaf node
            continue;
        }
        old_height = tmp_node->height;
        // left subtree is heavier
        if (tmp_node->left->height - tmp_node->right->height == 2) {
            if (tmp_node->left->left->height - tmp_node->right->height == 1) {
                // left subtree's left subtree is heavier, rotate right
                MAX_INTERVAL_TREE_FUNC(push_down_sum_rotate_right)(tmp_node);
                tmp_node->right->height = tmp_node->right->left->height + 1;
                tmp_node->height = tmp_node->right->height + 1;
            } else {
                // left subtree's right subtree is heavier, rotate left then right
                MAX_INTERVAL_TREE_FUNC(push_down_sum_rotate_left)(tmp_node->left);
                MAX_INTERVAL_TREE_FUNC(push_down_sum_rotate_right)(tmp_node);
                tmp_height = tmp_node->left->left->height;
                tmp_node->left->height = tmp_height + 1;
                tmp_node->right->height = tmp_height + 1;
                tmp_node->height = tmp_height + 2;
            }
        // right subtree is heavier
        } else if (tmp_node->right->height - tmp_node->left->height == 2) {
            if (tmp_node->right->right->height - tmp_node->left->height == 1) {
                // right subtree's right subtree is heavier, rotate left
                MAX_INTERVAL_TREE_FUNC(push_down_sum_rotate_left)(tmp_node);
                tmp_node->left->height = tmp_node->left->right->height + 1;
                tmp_node->height = tmp_node->left->height + 1;
            } else {
                // right subtree's left subtree is heavier, rotate right then left
                MAX_INTERVAL_TREE_FUNC(push_down_sum_rotate_right)(tmp_node->right);
                MAX_INTERVAL_TREE_FUNC(push_down_sum_rotate_left)(tmp_node);
                tmp_height = tmp_node->right->right->height;
                tmp_node->left->height = tmp_height + 1;
                tmp_node->right->height = tmp_height + 1;
                tmp_node->height = tmp_height + 2;
            }
        } else {
            // update height, no rotation needed
            if (tmp_node->left->height > tmp_node->right->height) {
                tmp_node->height = tmp_node->left->height + 1;
            } else {
                tmp_node->height = tmp_node->right->height + 1;
            }
        }
        if (tmp_node->height == old_height) {
            // terminate early if nothing changed heights
            done = true;
        }
    }
    return true;
}


// AVL tree insert with summand
bool MAX_INTERVAL_TREE_FUNC(insert)(MAX_INTERVAL_TREE_NAME *tree, MAX_INTERVAL_TREE_KEY_TYPE key, void *value) {
    if (tree == NULL) return false;
    MAX_INTERVAL_TREE_NODE *node = tree->root;
    MAX_INTERVAL_TREE_TYPED(node_memory_pool) *pool = tree->pool;

    MAX_INTERVAL_TREE_NODE *tmp_node;
    bool done = false;
    if (node->left == NULL) {
        // empty tree
        node->left = (MAX_INTERVAL_TREE_NODE *)value;
        node->key = key;
        node->height = 0;
        node->summand = (MAX_INTERVAL_TREE_SUM_TYPE)0;
        node->partial_sum = (MAX_INTERVAL_TREE_SUM_TYPE)0;
        node->right = NULL;
    } else {
        // non-empty tree
        MAX_INTERVAL_TREE_TYPED(stack_t) stack = (MAX_INTERVAL_TREE_TYPED(stack_t)){
            .stack = {NULL},
            .stack_size = 0
        };
        tmp_node = MAX_INTERVAL_TREE_TYPED(candidate_leaf)(node, key, &stack);
        if (MAX_INTERVAL_TREE_KEY_EQUALS(key, tmp_node->key)) {
            // key already exists
            return false;
        }
        // tmp_node is the leaf that will become the parent of the new leaf
        MAX_INTERVAL_TREE_NODE *old_leaf = MAX_INTERVAL_TREE_NODE_MEMORY_POOL_FUNC(get)(pool);
        if (old_leaf == NULL) return false;
        old_leaf->key = tmp_node->key;
        old_leaf->left = tmp_node->left;
        old_leaf->right = NULL;
        old_leaf->height = 0;
        old_leaf->summand = (MAX_INTERVAL_TREE_SUM_TYPE)0;
        old_leaf->partial_sum = tmp_node->partial_sum;
        MAX_INTERVAL_TREE_NODE *new_leaf = MAX_INTERVAL_TREE_NODE_MEMORY_POOL_FUNC(get)(pool);
        if (new_leaf == NULL) {
            MAX_INTERVAL_TREE_NODE_MEMORY_POOL_FUNC(release)(pool, old_leaf);
            return false;
        }
        new_leaf->key = key;
        new_leaf->left = (MAX_INTERVAL_TREE_NODE *)value;
        new_leaf->right = NULL;
        new_leaf->height = 0;
        new_leaf->summand = (MAX_INTERVAL_TREE_SUM_TYPE)0;
        new_leaf->partial_sum = tmp_node->partial_sum;
        if (MAX_INTERVAL_TREE_KEY_LESS_THAN(tmp_node->key, key)) {
            tmp_node->left = old_leaf;
            tmp_node->right = new_leaf;
            tmp_node->key = key;
        } else {
            tmp_node->left = new_leaf;
            tmp_node->right = old_leaf;
        }
        tmp_node->height = 1;
        return MAX_INTERVAL_TREE_FUNC(rebalance)(&stack);
    }
    return true;
}

void *MAX_INTERVAL_TREE_FUNC(delete)(MAX_INTERVAL_TREE_NAME *tree, MAX_INTERVAL_TREE_KEY_TYPE key) {
    if (tree == NULL) return NULL;
    MAX_INTERVAL_TREE_NODE *node = tree->root;
    MAX_INTERVAL_TREE_TYPED(node_memory_pool) *pool = tree->pool;

    MAX_INTERVAL_TREE_NODE *tmp_node, *upper_node, *other_node;
    void *deleted_value;
    if (node->left == NULL) {
        return NULL;
    } else if (node->right == NULL) {
        if (MAX_INTERVAL_TREE_KEY_EQUALS(key, node->key)) {
            deleted_value = (void *)node->left;
            node->left = NULL;
            return deleted_value;
        } else {
            return NULL;
        }
    } else {
        tmp_node = node;
        MAX_INTERVAL_TREE_TYPED(stack_t) stack = (MAX_INTERVAL_TREE_TYPED(stack_t)){
            .stack = {NULL},
            .stack_size = 0
        };
        while (MAX_INTERVAL_TREE_FUNC(node_is_internal)(tmp_node)) {
            if (!MAX_INTERVAL_TREE_FUNC(stack_push(&stack, tmp_node))) return NULL;
            upper_node = tmp_node;
            if (MAX_INTERVAL_TREE_KEY_LESS_THAN(key, tmp_node->key)) {
                tmp_node = upper_node->left;
                other_node = upper_node->right;
            } else {
                tmp_node = upper_node->right;
                other_node = upper_node->left;
            }
        }
        if (!MAX_INTERVAL_TREE_KEY_EQUALS(key, tmp_node->key)) {
            return NULL;
        } else {
            upper_node->key = other_node->key;
            upper_node->left = other_node->left;
            upper_node->right = other_node->right;
            upper_node->height = other_node->height;
            deleted_value = (void *)tmp_node->left;
            // remove upper_node from the stack since it's now a leaf node
            MAX_INTERVAL_TREE_FUNC(stack_pop)(&stack);

            MAX_INTERVAL_TREE_NODE_MEMORY_POOL_FUNC(release)(pool, tmp_node);
            MAX_INTERVAL_TREE_NODE_MEMORY_POOL_FUNC(release)(pool, other_node);
            if (!MAX_INTERVAL_TREE_FUNC(rebalance)(&stack)) {
                return NULL;
            }
            return deleted_value;
        }
    }

}

MAX_INTERVAL_TREE_NAME *MAX_INTERVAL_TREE_FUNC(new)(void) {
    MAX_INTERVAL_TREE_NAME *tree = malloc(sizeof(MAX_INTERVAL_TREE_NAME));
    if (tree == NULL) return NULL;
    tree->pool = MAX_INTERVAL_TREE_NODE_MEMORY_POOL_FUNC(new)();
    if (tree->pool == NULL) {
        free(tree);
        return NULL;
    }
    tree->root = MAX_INTERVAL_TREE_NODE_MEMORY_POOL_FUNC(get)(tree->pool);
    if (tree->root == NULL) {
        MAX_INTERVAL_TREE_NODE_MEMORY_POOL_FUNC(destroy)(tree->pool);
        free(tree);
        return NULL;
    }
    tree->root->summand = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    // need key for -inf, use root as non-NULL object pointer
    MAX_INTERVAL_TREE_FUNC(insert)(tree, MAX_INTERVAL_TREE_NEGATIVE_INFINITY, (void *)tree->root);
    return tree;
}

MAX_INTERVAL_TREE_SUM_TYPE MAX_INTERVAL_TREE_FUNC(max_value_interval)(MAX_INTERVAL_TREE_NAME *tree, MAX_INTERVAL_TREE_KEY_TYPE start, MAX_INTERVAL_TREE_KEY_TYPE end) {
    if (tree == NULL || tree->root == NULL || tree->root->left == NULL) return (MAX_INTERVAL_TREE_SUM_TYPE)0;

    MAX_INTERVAL_TREE_SUM_TYPE current_max = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    MAX_INTERVAL_TREE_SUM_TYPE tmp_sum = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    MAX_INTERVAL_TREE_SUM_TYPE sum = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    MAX_INTERVAL_TREE_SUM_TYPE left_sum = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    MAX_INTERVAL_TREE_SUM_TYPE right_sum = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    bool first = true;
    MAX_INTERVAL_TREE_NODE *current_node = tree->root;
    MAX_INTERVAL_TREE_NODE *left_path = NULL;
    MAX_INTERVAL_TREE_NODE *right_path = NULL;

    while (!MAX_INTERVAL_TREE_FUNC(node_is_leaf)(current_node)) {
        if (MAX_INTERVAL_TREE_KEY_LESS_THAN(end, current_node->key)) {
            // go left, start < end < key
            current_node = current_node->left;
        } else if (MAX_INTERVAL_TREE_KEY_LESS_THAN(current_node->key, start)) {
            // go right, key < end < start
            current_node = current_node->right;
        } else if (MAX_INTERVAL_TREE_KEY_LESS_THAN(start, current_node->key) && MAX_INTERVAL_TREE_KEY_LESS_THAN(current_node->key, end)) {
            // split: start < key < end, both right and left
            right_path = current_node->right;
            left_path = current_node->left;
            break;
        } else if (MAX_INTERVAL_TREE_KEY_EQUALS(current_node->key, start)) {
            right_path = current_node->right;
            // no left
            break;
        } else {
            // current_node->key == end, so start < key = end
            left_path = current_node->left;
            // no right
            break;
        }
    }

    if ((left_path == NULL) && (right_path == NULL)) {
        current_max = sum + current_node->summand + current_node->partial_sum;
    }
    left_sum = right_sum = sum;
    if (left_path != NULL) {
        while (!MAX_INTERVAL_TREE_FUNC(node_is_leaf)(left_path)) {
            left_sum += left_path->summand;
            if (MAX_INTERVAL_TREE_KEY_LESS_THAN(start, left_path->key)) {
                tmp_sum = left_sum + left_path->right->summand + left_path->right->partial_sum;
                if (first || tmp_sum > current_max) {
                    current_max = tmp_sum;
                    first = false;
                }
                left_path = left_path->left;
            } else if (MAX_INTERVAL_TREE_KEY_EQUALS(start, left_path->key)) {
                tmp_sum = left_sum + left_path->right->summand + left_path->right->partial_sum;
                if (first || tmp_sum > current_max) {
                    current_max = tmp_sum;
                    first = false;
                }
                // no further descent needed
                break;
            } else {
                // go right, no node selected
                left_path = left_path->right;
            }
        }
        // left leaf needs to be checked if reached
        if (MAX_INTERVAL_TREE_FUNC(node_is_leaf)(left_path)) {
            tmp_sum = left_sum + left_path->summand + left_path->partial_sum;
            if (first || tmp_sum > current_max) {
                current_max = tmp_sum;
                first = false;
            }
        }
    }
    if (right_path != NULL) {
        // now follow the path right of endpoint end
        while (!MAX_INTERVAL_TREE_FUNC(node_is_leaf)(right_path)) {
            right_sum += right_path->summand;
            if (MAX_INTERVAL_TREE_KEY_LESS_THAN(right_path->key, end)) {
                // left node possibly contributes
                tmp_sum = right_sum + right_path->left->summand + right_path->left->partial_sum;
                if (first || tmp_sum > current_max) {
                    current_max = tmp_sum;
                    first = false;
                }
                right_path = right_path->right;
            } else if (MAX_INTERVAL_TREE_KEY_EQUALS(right_path->key, end)) {
                tmp_sum = right_sum + right_path->left->summand + right_path->left->partial_sum;
                if (first || tmp_sum > current_max) {
                    current_max = tmp_sum;
                    first = false;
                }
                // no further descent needed
                break;
            } else {
                // go left, no nodes selected
                right_path = right_path->left;
            }
        }
        if (MAX_INTERVAL_TREE_FUNC(node_is_leaf)(right_path) && MAX_INTERVAL_TREE_KEY_LESS_THAN(right_path->key, end)) {
            tmp_sum = right_sum + right_path->summand + right_path->partial_sum;
            if (first || tmp_sum > current_max) {
                current_max = tmp_sum;
                first = false;
            }
        }
    }
    return current_max;
}

bool MAX_INTERVAL_TREE_FUNC(insert_interval)(MAX_INTERVAL_TREE_NAME *tree,
                                                      MAX_INTERVAL_TREE_KEY_TYPE start,
                                                      MAX_INTERVAL_TREE_KEY_TYPE end,
                                                      MAX_INTERVAL_TREE_SUM_TYPE weight) {

    if (tree == NULL) return false;
    if (MAX_INTERVAL_TREE_FUNC(get)(tree->root, start) == NULL) {
        MAX_INTERVAL_TREE_FUNC(insert)(tree, start, (void *)tree->root);
    }
    if (MAX_INTERVAL_TREE_FUNC(get)(tree->root, end) == NULL) {
        MAX_INTERVAL_TREE_FUNC(insert)(tree, end, (void *)tree->root);
    }

    MAX_INTERVAL_TREE_NODE *tmp_node = tree->root;
    MAX_INTERVAL_TREE_TYPED(stack_t) stack = (MAX_INTERVAL_TREE_TYPED(stack_t)){
        .stack = {NULL},
        .stack_size = 0
    };

    // follow search path for start
    while (!MAX_INTERVAL_TREE_FUNC(node_is_leaf)(tmp_node)) {
        if (!MAX_INTERVAL_TREE_FUNC(stack_push(&stack, tmp_node))) return NULL;
        // add weight to everything right of path
        if (MAX_INTERVAL_TREE_KEY_LESS_THAN(start, tmp_node->key)) {
            tmp_node->right->summand += weight;
            tmp_node = tmp_node->left;
        } else {
            tmp_node = tmp_node->right;
        }
    }

    MAX_INTERVAL_TREE_SUM_TYPE left_sum = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    MAX_INTERVAL_TREE_SUM_TYPE right_sum = (MAX_INTERVAL_TREE_SUM_TYPE)0;
    // leaf with key start
    tmp_node->summand += weight;
    while (!MAX_INTERVAL_TREE_FUNC(stack_empty)(&stack)) {
        tmp_node = MAX_INTERVAL_TREE_FUNC(stack_pop)(&stack);
        left_sum = tmp_node->left->summand + tmp_node->left->partial_sum;
        right_sum = tmp_node->right->summand + tmp_node->right->partial_sum;
        tmp_node->partial_sum = (left_sum > right_sum) ? left_sum : right_sum;
    }
    // now same steps for end with weight -w
    tmp_node = tree->root;
    // follow search path for end
    while (!MAX_INTERVAL_TREE_FUNC(node_is_leaf)(tmp_node)) {
        if (!MAX_INTERVAL_TREE_FUNC(stack_push(&stack, tmp_node))) return NULL;
        // subtract weight from everything right of path
        if (MAX_INTERVAL_TREE_KEY_LESS_THAN(end, tmp_node->key)) {
            tmp_node->right->summand -= weight;
            tmp_node = tmp_node->left;
        } else {
            tmp_node = tmp_node->right;
        }
    }

    // leaf with key end
    tmp_node->summand -= weight;
    // and now correct nodes along the search path for end
    while (!MAX_INTERVAL_TREE_FUNC(stack_empty)(&stack)) {
        tmp_node = MAX_INTERVAL_TREE_FUNC(stack_pop)(&stack);
        left_sum = tmp_node->left->summand + tmp_node->left->partial_sum;
        right_sum = tmp_node->right->summand + tmp_node->right->partial_sum;
        tmp_node->partial_sum = (left_sum > right_sum) ? left_sum : right_sum;
    }

    return true;
}


void MAX_INTERVAL_TREE_FUNC(destroy)(MAX_INTERVAL_TREE_NAME *tree) {
    if (tree == NULL) return;
    if (tree->pool != NULL) {
        MAX_INTERVAL_TREE_NODE_MEMORY_POOL_FUNC(destroy)(tree->pool);
    }
    free(tree);
}
