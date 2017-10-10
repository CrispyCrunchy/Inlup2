#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
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
  element_copy_fun copy;
  element_free_fun free;
  element_comp_fun comp;
  link_t *last;
};


list_t *list_new(element_copy_fun copy, element_free_fun free, element_comp_fun comp)
{
  list_t *new = calloc(1, sizeof(list_t));
  new->copy = copy;
  new->free = free;
  new->comp = comp;
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

void list_insert(list_t *list, int index, elem_t elem)
{
  link_t *tmp = list->first;
  link_t *newlink = calloc(1, sizeof(link_t));
  newlink->elem = &elem;
  int len = list_length(list);
  if (index > len)
    {
      index = len;
    }
  if (index < 0 && index > (0 - len))
    {
      index = len + index + 1;
    }
  if (index < (0 - len))
    {
      index = 0;
    }
  if (index == 0)
    {
      index = 1;
    }
  // 'list's copy function is non-NULL, it will be applied to elem and its result'??
  if (list->copy != NULL)
    {
      list->copy(elem);
    }
  else
    {
      for (int i = 1; i < index - 1; ++i)
        {
          tmp = tmp->next;
        } 
      newlink->next = tmp->next;
      tmp->next = newlink;
    }
}

void list_append(list_t *list, elem_t elem)
{
  list_insert(list, 0, elem);
}

void list_prepend(list_t *list, elem_t elem)
{
  int len = list_length(list);
  list_insert(list, len, elem);
}
  
void list_remove(list_t *list, int index, bool delete)
{
  int len = list_length(list);
  link_t *tmp = list->first;
  if (index > len)
    {
      index = len;
    }
  if (index < 0 && index > (0 - len))
    {
      index = len + index + 1;
    }
  if (index < (0 - len))
    {
      index = 0;
    }
  if (index == 0)
    {
      index = 1;
    }
  if (delete == true)
    {
      for (int i = 1; i < index - 1; ++i)
        {
          tmp = tmp->next;
        }
      tmp->next = tmp->next->next;
      // 'param delete if true, run list's free function on the removed element'
      list->free(*tmp->elem);   
    }
}

bool list_get(list_t *list, int index, elem_t *result)
{
  int len = list_length(list);
  link_t *tmp = list->first;
  if (index < 0)
    {
      index = len + index + 1;
    }
  if (index > len)
    {
      return false;
    }
  if (index == 0)
    {
      index = 1;
    }
  for (int i = 0; i < index - 1; ++i)
    {
      tmp = tmp->next;
    }
  result = tmp->elem;
  return true;
}


bool list_first(list_t *list, elem_t *result)
{
  return list_get(list, 0, result);
}

bool list_last(list_t *list, elem_t *result)
{
  return list_get(list, -1, result);
}

void list_delete(list_t *list, bool delete)
{
  int len = list_length(list);
  // 'param delete if true, use list's free function to free elements'
  if (delete == true)
    {
      for (int i = len; i > 0; --i)
        {
          list->free(*list->last->elem);
        } 
    }
}


bool list_apply(list_t *list, elem_apply_fun fun, void *data)
{
  int siz = list_length(list);
  link_t *tmp = list->first;
  bool result = false;
  bool t = false;
  for (int i = 0; i < siz; ++i)
    {
      t = fun(*tmp->elem, data);
      tmp = tmp->next;
      if (result == false)
        {
          result = t;
        }
    }
  return result;
}

int list_contains(list_t *list, elem_t elem)
{
  int len = list_length(list);
  // 'list's compare if non-NULL'??
  if (list->comp != NULL)
    {
      for (int i = 0; i <= len; ++i )
        {
          elem_t *result;
          bool list_exist = list_get(list, i, result);
          if (list_exist == true)
            {
              list->comp(elem, *result);
              return i;
            }
          else
            {
              return -1;
            }
        }
    }
}
