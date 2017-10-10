#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
//#include "utils.h"
#include "common.h"

typedef struct link link_t;

struct link
{
  elem_t *elem;
  struct link *next;
};

struct list
{
  link_t *first;
  link_t *last;
};


list_t *list_new(element_copy_fun copy, element_free_fun free, element_comp_fun compare)
{
  list_t *new = calloc(1, sizeof(list_t));
  return new;
}

int list_length(list_t *list)
{
  int len = 0;
  link_t *tmp = list->first;
  
  while (tmp != NULL)
    {
      ++len;
      tmp = tmp->next;
    }
  
  return len;
}

void list_insert(list_t *list, int index, elem_t elem);
void list_append(list_t *list, elem_t elem);
void list_prepend(list_t *list, elem_t elem);
void list_remove(list_t *list, int index, bool delete);
bool list_get(list_t *list, int index, elem_t *result);
bool list_first(list_t *list, elem_t *result);
bool list_last(list_t *list, elem_t *result);

void list_delete(list_t *list, bool delete);

bool list_apply(list_t *list, elem_apply_fun fun, void *data)
{
  int siz = list_length(list);
  link_t *tmp = list->first;
  for (int i = 0; i < siz; ++i)
    {
      fun(*tmp->elem, data);
      tmp = tmp->next;
    }
  return true;
}

int list_contains(list_t *list, elem_t elem);

