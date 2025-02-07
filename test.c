#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "greatest/greatest.h"

#define MAX_INTERVAL_TREE_NAME max_interval_tree_int32
#define MAX_INTERVAL_TREE_KEY_TYPE int32_t
#define MAX_INTERVAL_TREE_SUM_TYPE float
#define MAX_INTERVAL_TREE_NEGATIVE_INFINITY INT32_MIN
#include "max_interval_tree.h"
#undef MAX_INTERVAL_TREE_NAME
#undef MAX_INTERVAL_TREE_KEY_TYPE
#undef MAX_INTERVAL_TREE_SUM_TYPE
#undef MAX_INTERVAL_TREE_NEGATIVE_INFINITY

TEST test_max_interval_tree(void) {
    max_interval_tree_int32 *tree = max_interval_tree_int32_new();

    max_interval_tree_int32_insert_interval(tree, 3, 4, 1.9f);
    max_interval_tree_int32_insert_interval(tree, 2, 6, -0.3f);
    max_interval_tree_int32_insert_interval(tree, 1, 9, 0.3f);
    max_interval_tree_int32_insert_interval(tree, 5, 7, 1.4f);
    max_interval_tree_int32_insert_interval(tree, 8, 11, 0.4f);
    max_interval_tree_int32_insert_interval(tree, 10, 13, 1.1f);
    max_interval_tree_int32_insert_interval(tree, 12, 14, -1.2f);
    max_interval_tree_int32_insert_interval(tree, 12, 15, 0.7f);

    float max = max_interval_tree_int32_max_value_interval(tree, 6, 7);
    printf("max at [6,7[ is %f\n", max);

    max = max_interval_tree_int32_max_value_interval(tree, 8, 9);
    printf("max at [8,9[ is %f\n", max);

    max = max_interval_tree_int32_max_value_interval(tree, 12, 13);
    printf("max at [12, 13[ is %f\n", max);

    max_interval_tree_int32_destroy(tree);
    PASS();
}



/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

    RUN_TEST(test_max_interval_tree);

    GREATEST_MAIN_END();        /* display results */
}