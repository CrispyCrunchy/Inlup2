#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Automated.h>
#include "list.h"
#include "common.h"

typedef struct link link_t;


void test_list_new(void)
{
  list_t *new = list_new(NULL, NULL, NULL);
  CU_ASSERT_TRUE(list_length(new) == 0);
  list_delete(new, false);
}

void test_list_length(void)
{
  list_t *list = list_new(NULL, NULL, NULL);

  elem_t elem1 = { .i = 1};
  
  for (int i = 0; i < 10; ++i)
    {
      list_append(list, elem1);
    }
  CU_ASSERT_TRUE(list_length(list) == 10);

  for (int i = 0; i < 10; ++i)
    {
      list_append(list, elem1);
    }
  CU_ASSERT_TRUE(list_length(list) == 20);

  list_remove(list, 1, true);
  CU_ASSERT_TRUE(list_length(list) == 19);

  list_insert(list, 1, elem1);
  CU_ASSERT_TRUE(list_length(list) == 20);

  list_delete(list, true);
}

void test_list_insert(void)
{
  list_t *list = list_new();

  elem_t elem1 = { .i = 1};
  elem_t elem1 = { .i = 1};
  
  for (int i = 0; i < 10; ++i)
    {
      list_insert(list, 1, elem1);
      CU_TEST(list_length(list) == i + 1)
    }

  for (int j = 0; j < 10; ++j)
    {
      list_insert(list, 4, );
      CU_TEST(list_length(list) == j + 11)
    }

  for (int k = 0; k < 10; ++k)
    {
      list_insert(list, -4, );
      CU_TEST(list_length(list) == k + 21)
    }

  list_insert(list, -1, );
  CU_ASSERT_TRUE(list_length(list) == 31);

  list_insert(list, -99, );
  CU_ASSERT_TRUE(list_length(list) == 32);

  list_delete(list, true);
}


void test_list_contains(void);
void test_list_append(void);
void test_list_prepend(void);



void test_list_get(void);
void test_list_first(void);
void test_list_last(void);


void test_list_delete(void);
void test_list_remove(void);

void test_list_apply(void);



int main(int argc, char *argv[argc])
{
  // Initialise
  CU_initialize_registry();

  // Set up suites and tests
  CU_pSuite creation = CU_add_suite("Test creation and size", NULL, NULL);
  CU_add_test(creation, "Creation", test_list_new);
  CU_add_test(creation, "Size", test_list_length);

  CU_pSuite insertion = CU_add_suite("Test insertion (insert, append, prepend)", NULL, NULL);
  CU_add_test(insertion, "Insert", test_list_insert);
  CU_add_test(insertion, "Append", test_list_append);
  CU_add_test(insertion, "Prepend", test_list_prepend);

  CU_pSuite get = CU_add_suite("Test get (get, contain,  get first, get last)", NULL, NULL);
  CU_add_test(get, "Get", test_list_get);
  CU_add_test(get, "Contain", test_list_contains);
  CU_add_test(get, "Get first", test_list_first);
  CU_add_test(get, "Get last", test_list_last);

  CU_pSuite removal = CU_add_suite("Test removal", NULL, NULL);
  CU_add_test(removal, "Delete entire list", test_list_delete);
  CU_add_test(removal, "Remove element in list", test_list_remove);

  CU_pSuite apply = CU_add_suite("Test apply", NULL, NULL);
  CU_add_test(apply, "Apply", test_list_apply);
  
  
  // Actually run tests
  CU_basic_run_tests();
  
  // Tear down
  CU_cleanup_registry();
  return CU_get_error();
}
