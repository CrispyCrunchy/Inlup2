#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Automated.h>
#include "tree.h"

void test_tree_new(void)
{
  tree_t *new = tree_new(NULL, NULL, NULL, NULL);
  CU_ASSERT_TRUE(tree_size(new) == 0);
  tree_delete(new, true, true);
}

void test_tree_sort(void);
void test_tree_size(void)
{
  tree_t *tree = tree_new(NULL, NULL, NULL, NULL);

  elem_t result = { .i = 0};

  tree_key_t key1 = { .p = "d"};
  elem_t elem1 = { .i = 1};
  
  tree_key_t key2 = { .p = "e"};
  elem_t elem2 = { .i = 2};

  tree_key_t key3 = { .p = "f"};
  elem_t elem3 = { .i = 3};

  tree_key_t key4 = { .p = "a"};
  elem_t elem4 = { .i = 4};
  
  tree_insert(tree, key1, elem1);

  CU_ASSERT_TRUE(tree_size(tree) == 1);

  tree_insert(tree, key2, elem2);
  tree_insert(tree, key3, elem3);

  CU_ASSERT_TRUE(tree_size(tree) == 3);

  tree_insert(tree, key4, elem4);

  CU_ASSERT_TRUE(tree_size(tree) == 4);

  tree_remove(tree, key4, &result);

  CU_ASSERT_TRUE(tree_size(tree) == 3);

  tree_remove(tree, key2, &result);

  CU_ASSERT_TRUE(tree_size(tree) == 2);
  
  tree_delete(tree, true, true);
}

void test_tree_depth(void)
{
  tree_t *tree = tree_new(NULL, NULL, NULL, NULL);

  elem_t result = { .i = 0};

  tree_key_t key1 = { .p = "d"};
  elem_t elem1 = { .i = 1};
  
  tree_key_t key2 = { .p = "e"};
  elem_t elem2 = { .i = 2};

  tree_key_t key3 = { .p = "f"};
  elem_t elem3 = { .i = 3};

  tree_key_t key4 = { .p = "a"};
  elem_t elem4 = { .i = 4};
  
  tree_insert(tree, key1, elem1);

  CU_ASSERT_TRUE(tree_size(tree) == 1);

  tree_insert(tree, key2, elem2);
  tree_insert(tree, key3, elem3);

  CU_ASSERT_TRUE(tree_size(tree) == 3);

  tree_insert(tree, key4, elem4);

  CU_ASSERT_TRUE(tree_size(tree) == 3);

  tree_remove(tree, key4, &result);

  CU_ASSERT_TRUE(tree_size(tree) == 3);

  tree_remove(tree, key2, &result);

  CU_ASSERT_TRUE(tree_size(tree) == 2);
  
  tree_delete(tree, true, true);
}

void test_tree_insert(void)
{
  tree_t *tree = tree_new(NULL, NULL, NULL, NULL);

  elem_t result = { .i = 0};

  tree_key_t key1 = { .p = "d"};
  elem_t elem1 = { .i = 1};
  
  tree_key_t key2 = { .p = "e"};
  elem_t elem2 = { .i = 2};

  tree_key_t key3 = { .p = "f"};
  elem_t elem3 = { .i = 3};

  tree_key_t key4 = { .p = "a"};
  elem_t elem4 = { .i = 4};
  
  tree_insert(tree, key1, elem1);
  tree_insert(tree, key2, elem2);
  tree_insert(tree, key3, elem3);
  tree_insert(tree, key4, elem4);

  tree_get(tree, key1, &result);
  CU_ASSERT_EQUAL(elem1.i, 1);

  tree_get(tree, key4, &result);
  CU_ASSERT_EQUAL(elem4.i, 4);
  
  tree_delete(tree, true, true);
}

void test_tree_get(void)
{
  tree_t *tree = tree_new(NULL, NULL, NULL, NULL);

  elem_t result = { .i = 0};

  tree_key_t key1 = { .p = "d"};
  elem_t elem1 = { .i = 1};
  
  tree_key_t key2 = { .p = "e"};
  elem_t elem2 = { .i = 2};

  tree_key_t key3 = { .p = "f"};
  elem_t elem3 = { .i = 3};

  tree_key_t key4 = { .p = "a"};
  elem_t elem4 = { .i = 4};

  tree_key_t key5 = { .p = "q"};

  tree_insert(tree, key4, elem4);
  tree_insert(tree, key3, elem3);
  tree_insert(tree, key2, elem2);
  tree_insert(tree, key1, elem1);

  CU_ASSERT_TRUE(tree_get(tree, key1, &result));
  CU_ASSERT_FALSE(tree_get(tree, key5, &result));

  tree_get(tree, key1, &result);
  CU_ASSERT_EQUAL(result.i, 1);

  tree_get(tree, key3, &result);
  CU_ASSERT_EQUAL(result.i, 3);
  // en koll vad result är?

  tree_delete(tree, true, true);
}

void test_tree_has_key(void)
{
  tree_t *tree = tree_new(NULL, NULL, NULL, NULL);

  tree_key_t key1 = { .p = "d"};
  elem_t elem1 = { .i = 1};
  
  tree_key_t key2 = { .p = "e"};
  elem_t elem2 = { .i = 2};

  tree_key_t key3 = { .p = "f"};
  elem_t elem3 = { .i = 3};

  tree_key_t key4 = { .p = "a"};
  elem_t elem4 = { .i = 4};

  tree_key_t key5 = { .p = "q"};

  tree_insert(tree, key4, elem4);
  tree_insert(tree, key3, elem3);
  tree_insert(tree, key2, elem2);
  tree_insert(tree, key1, elem1);

  CU_ASSERT_TRUE(tree_has_key(tree, key1));
  CU_ASSERT_FALSE(tree_has_key(tree, key5));

  tree_delete(tree, true, true);
}

void test_tree_delete(void)
{
  tree_t *tree = tree_new(NULL, NULL, NULL, NULL);
  tree_delete(tree, true, true);

  CU_ASSERT_PTR_NULL(tree);
}

void test_tree_remove(void)
{
  tree_t *tree = tree_new(NULL, NULL, NULL, NULL);

  elem_t result = { .i = 0};

  tree_key_t key1 = { .p = "d"};
  elem_t elem1 = { .i = 1};
  
  tree_key_t key2 = { .p = "e"};
  elem_t elem2 = { .i = 2};

  tree_key_t key3 = { .p = "f"};
  elem_t elem3 = { .i = 3};

  tree_key_t key4 = { .p = "a"};
  elem_t elem4 = { .i = 4};
  
  tree_insert(tree, key1, elem1);
  tree_insert(tree, key2, elem2);
  tree_insert(tree, key3, elem3);
  tree_insert(tree, key4, elem4);

  tree_remove(tree, key1, &result);
  tree_remove(tree, key4, &result);
  CU_ASSERT(tree_size(tree) == 2);

  tree_remove(tree, key2, &result);
  CU_ASSERT(tree_size(tree) == 1);
  
  tree_delete(tree, true, true);
}

void test_tree_elements(void);
void test_tree_keys(void);

void test_tree_update(void);

void test_tree_apply(void);




int main(int argc, char *argv[argc])
{
  // Initialise
  CU_initialize_registry();

  // Set up suites and tests
  CU_pSuite creation = CU_add_suite("Test creation, sorting, size and depth", NULL, NULL);
  CU_add_test(creation, "Creation", test_tree_new);
  CU_add_test(creation, "Sorting", test_tree_sort);
  CU_add_test(creation, "Size", test_tree_size);
  CU_add_test(creation, "Depth", test_tree_depth);

  CU_pSuite insertion = CU_add_suite("Test insertion", NULL, NULL);
  CU_add_test(insertion, "Insert", test_tree_insert);

  CU_pSuite get = CU_add_suite("Test get (get, tree has key)", NULL, NULL);
  CU_add_test(get, "Get", test_tree_get);
  CU_add_test(get, "Contain", test_tree_has_key);

  CU_pSuite removal = CU_add_suite("Test removal", NULL, NULL);
  CU_add_test(removal, "Delete entire tree", test_tree_delete);
  CU_add_test(removal, "Remove element in tree", test_tree_remove);

  CU_pSuite content = CU_add_suite("Test content", NULL, NULL);
  CU_add_test(content, "Keys", test_tree_keys);
  CU_add_test(content, "Elements", test_tree_elements);

 CU_pSuite update = CU_add_suite("Test update", NULL, NULL);
  CU_add_test(update, "Update", test_tree_update);
  
  CU_pSuite apply = CU_add_suite("Test apply", NULL, NULL);
  CU_add_test(apply, "Apply", test_tree_apply);
  
  
  // Actually run tests
  CU_basic_run_tests();
  
  // Tear down
  CU_cleanup_registry();
  return CU_get_error();
}
