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
  list_t *list = list_new(NULL, NULL, NULL);

  CU_TEST(list_length(list) == 0);

  elem_t elem1 = { .i = 1};
  elem_t elem2 = { .i = 2};
  elem_t elem3 = { .i = 3};
  elem_t elem4 = { .i = 4};

  elem_t result = { .i = 0};
  
  for (int i = 0; i < 10; ++i)
    {
      list_insert(list, 1, elem1);
      CU_TEST(list_length(list) == i + 1)
        }

  list_get(list, 1, &result);
  CU_ASSERT_EQUAL(result.i, 1);
  // en koll vad result är

  for (int j = 0; j < 10; ++j)
    {
      list_insert(list, 4, elem1);
      CU_TEST(list_length(list) == j + 11)
        }

  for (int k = 0; k < 10; ++k)
    {
      list_insert(list, -4, elem2);
      CU_TEST(list_length(list) == k + 21)
        }

  list_insert(list, -1, elem3);
  CU_ASSERT_TRUE(list_length(list) == 31);

  list_insert(list, -99, elem4);
  CU_ASSERT_TRUE(list_length(list) == 32);

  list_delete(list, true);
}


void test_list_contains(void)
{
  list_t *list = list_new(NULL, NULL, NULL);

  elem_t elem1 = { .i = 1};
  elem_t elem2 = { .i = 2};
  elem_t elem3 = { .i = 3};
  elem_t elem4 = { .i = 4};

  list_insert(list, 1, elem4);
  list_insert(list, 1, elem3);
  list_insert(list, 1, elem2);
  list_insert(list, 1, elem1);
  
  CU_TEST(list_contains(list, elem1) == 1);
  CU_TEST(list_contains(list, elem2) == 2);
  CU_TEST(list_contains(list, elem3) == 3);
  CU_TEST(list_contains(list, elem4) == 4);
  
  list_delete(list, true);
  
}


void test_list_append(void)
{
  list_t *list = list_new(NULL, NULL, NULL);

  elem_t elem1 = { .i = 1};
  elem_t elem2 = { .i = 2};
  elem_t elem3 = { .i = 3};
  elem_t elem4 = { .i = 4};
  
  for (int i = 0; i < 10; ++i)
    {
      list_insert(list, 1, elem1);
    }

  list_append(list, elem2);
  CU_TEST(list_contains(list, elem2) == 10); // 10 eller 11?

  list_append(list, elem3);
  list_append(list, elem4);
  CU_TEST(list_contains(list, elem4) == 12);

  list_delete(list, true);
}
  
void test_list_prepend(void)
{
  list_t *list = list_new(NULL, NULL, NULL);

  elem_t elem1 = { .i = 1};
  elem_t elem2 = { .i = 2};
  elem_t elem3 = { .i = 3};
  elem_t elem4 = { .i = 4};
  
  for (int i = 0; i < 10; ++i)
    {
      list_insert(list, 1, elem1);
    }

  list_prepend(list, elem2);
  CU_TEST(list_contains(list, elem2) == 1);

  list_prepend(list, elem3);
  list_prepend(list, elem4);
  CU_TEST(list_contains(list, elem3) != 1);
  CU_TEST(list_contains(list, elem4) == 1);

  list_delete(list, true);

}

void test_list_get(void)
{
  list_t *list = list_new(NULL, NULL, NULL);

  elem_t elem1 = { .i = 1};
  elem_t elem2 = { .i = 2};
  elem_t elem3 = { .i = 3};
  elem_t elem4 = { .i = 4};

  elem_t result = { .i = 0};

  list_insert(list, 1, elem4);
  list_insert(list, 1, elem3);
  list_insert(list, 1, elem2);
  list_insert(list, 1, elem1);

  CU_ASSERT_TRUE(list_get(list, 1, &result));
  CU_ASSERT_FALSE(list_get(list, 5, &result));

  list_get(list, 1, &result);
  CU_ASSERT_EQUAL(result.i, 1);
  // en koll vad result är?

  list_get(list, 3, &result);
  CU_ASSERT_EQUAL(result.i, 3);
  // en koll vad result är?

  list_delete(list, true);
  
}

void test_list_first(void)
{
  list_t *list = list_new(NULL, NULL, NULL);

  elem_t elem1 = { .i = 1};
  elem_t elem2 = { .i = 2};
  elem_t elem3 = { .i = 3};
  elem_t elem4 = { .i = 4};

  elem_t result = { .i = 0};

  list_insert(list, 1, elem4);
  list_insert(list, 1, elem3);

  list_first(list, &result);
  CU_ASSERT_EQUAL(result.i, 3);
  // en koll vad result är?
  
  list_insert(list, 1, elem2);
  list_insert(list, 1, elem1);

  CU_ASSERT_TRUE(list_first(list, &result) == true);

  list_first(list, &result);
  CU_ASSERT_EQUAL(result.i, 1);
  // en koll vad result är?

}

void test_list_last(void)
{
  list_t *list = list_new(NULL, NULL, NULL);

  elem_t elem1 = { .i = 1};
  elem_t elem2 = { .i = 2};
  elem_t elem3 = { .i = 3};
  elem_t elem4 = { .i = 4};
  elem_t elem5 = { .i = 5};

  elem_t result = { .i = 0};

  list_insert(list, 1, elem4);
  list_insert(list, 1, elem3);

  list_last(list, &result);
  CU_ASSERT_EQUAL(result.i, 4);
  // en koll vad result är?
  
  list_insert(list, 1, elem2);
  list_insert(list, 1, elem1);

  CU_ASSERT_TRUE(list_last(list, &result) == true);

  list_last(list, &result);
  CU_ASSERT_EQUAL(result.i, 4);
  // en koll vad result är?

  list_insert(list, 5, elem5);

  list_last(list, &result);
  CU_ASSERT_EQUAL(result.i, 5);
  // en koll vad result är?
}


void test_list_delete(void)
{
  list_t *list = list_new(NULL, NULL, NULL);
  list_delete(list, true);

  CU_ASSERT_PTR_NULL(list);
}

void test_list_remove(void)
{
  list_t *list = list_new(NULL, NULL, NULL);

  elem_t elem1 = { .i = 1};
  
  for (int i = 0; i < 10; ++i)
    {
      list_append(list, elem1);
    }
  CU_ASSERT_TRUE(list_length(list) == 10);

  list_remove(list, 1, true);
  CU_ASSERT_TRUE(list_length(list) == 9);

  
  list_delete(list, true);
}

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
