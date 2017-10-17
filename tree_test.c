#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Automated.h>
#include "tree.h"

void test_tree_new(void)
{
  tree_t *new = tree_new();
  CU_ASSERT_TRUE(tree_size(new) == 0);
  tree_delete(new, true, true);
}

void test_tree_sort(void);
void test_tree_size(void);
void test_tree_depth(void);

void test_tree_insert(void);

void test_tree_get(void);
void test_tree_has_key(void);

void test_tree_delete(void);
void test_tree_remove(void);

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
