#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "tree.h"
#include "list.h"
#include "common.h"
#include "db.h"

typedef struct shelf shelf_t;

struct shelf
{
  char *shelf_name;
  int amount;
};

typedef struct item item_t;

struct item
{
  char *desc;
  int price;
  list_t *shelves;
};

struct action
{
  int type; // NOTHING = 0, ADD = 1, REMOVE = 2, EDIT = 3
  item_t copy;
  item_t *orig;
};

shelf_t *elem_to_shelf(elem_t *pointer)
{
  void *void_pointer = pointer;
  shelf_t *shelf = void_pointer;
  return shelf;
}

elem_t *shelf_to_elem(shelf_t *shelf)
{
  void *pointer = shelf;
  elem_t *elem_pointer = pointer;
  return elem_pointer;
}

item_t *elem_to_item(elem_t *pointer)
{
  void *void_pointer = pointer;
  item_t *item = void_pointer;
  return item;
}

elem_t *item_to_elem(item_t *item)
{
  void *void_pointer = item;
  elem_t *elem_pointer = void_pointer;
  return elem_pointer;
}

elem_t *char_to_elem(char *character)
{
  void *void_pointer = character;
  elem_t *elem_pointer = void_pointer;
  return elem_pointer;
}

char *elem_to_char(elem_t *pointer)
{
  void *void_pointer = pointer;
  char *character = void_pointer;
  return character;
}

int compare_letter(char *first_string, char *second_string)
{
  int len1 = strlen(first_string);
  int len2 = strlen(second_string);
  int min = 0;
  if (len1 < len2)
    {
      min = len1;
    }
  else
    {
      min = len2;
    }
  char *f = calloc(len1 + 1, sizeof(char));
  char *s = calloc(len2 + 1, sizeof(char));
  for (int i = 0; i <= min; ++i)
    {
      char first = first_string[i];
      char second = second_string[i];
      first = toupper(first);
      second = toupper(second);
      f[i] = first;
      *(f+i) = first; 
      s[i] = second;
    }
  return strcmp(f, s);
}

void free_elem_list(list_t *list)
{
  free(list);
}

item_t copy_item(item_t *item)
{
  item_t *item_copy = calloc(1, sizeof(struct item));
  
  item_copy->desc = item->desc;
  item_copy->price = item->price;

  list_t *list_copy = list_new((element_copy_fun) copy_item,(element_free_fun) free_elem_list,(element_comp_fun) compare_letter);
  int siz = list_length(item->shelves);

  elem_t *result = NULL;
  shelf_t *tmp = NULL;
  
  for (int i = 0; i < siz; ++i)
    {
      list_get(item->shelves, i + 1, result);
      tmp = elem_to_shelf(result);
      shelf_t *shelf_copy = calloc(1, sizeof(shelf_t));
      
      shelf_copy->shelf_name = tmp->shelf_name;
      shelf_copy->amount = tmp->amount;

      elem_t *elem_shelf_copy = shelf_to_elem(shelf_copy);
      list_append(list_copy, *elem_shelf_copy);
    }
  
  item_copy->shelves = list_copy;

  return *item_copy;
}


void edit_savestate(item_t *item, struct action *savestate, int type)
{  
  savestate->type = type;
  savestate->copy = copy_item(item);
  savestate->orig = item;  
}

shelf_t *make_shelf(char *shelf, int amount)
{
  shelf_t *new_shelf = calloc(1, sizeof(struct shelf));
  new_shelf->shelf_name = shelf;
  new_shelf->amount = amount;
  return new_shelf;
}

item_t *make_item(char *desc, int price, list_t *shelves)
{
  item_t *item = calloc(1, sizeof(struct item));
  item->desc = desc;
  item->price = price;
  item->shelves = shelves;
  return item;
}

void print_shelf(item_t *item)
{
  int size = list_length(item->shelves);
  elem_t *result = NULL;
  shelf_t *shelf = NULL;
  for (int i = 1; i <= size; ++i)
    {
      list_get(item->shelves, i, result);
      shelf = elem_to_shelf(result);
      char *name = shelf->shelf_name;
      printf("%d. Hyllplats: %s\n", i, name);
    }
}

int tree_index(tree_t *tree)
{
  int screen_index = 1;
  tree_key_t *keys = tree_keys(tree);
  int size = tree_size(tree);
  
  for (int i = 0; i < size; ++i, ++screen_index)
    {
      printf("%d. %s\n", screen_index, elem_to_char(&keys[i]));
      if (screen_index == 20)
        {
          screen_index = 0;
          
          char answer = ask_question_yes_no("Visa nästa 20 varor, [J]a/[N]ej?");
          if (answer == 'N')
            {
              return (i - 1) / 20;
            }
        }
    }
  
  return (size - 1) / 20;
}


bool exist_shelf(list_t *master_list, char *shelf_name)
{
  int siz = list_length(master_list);
  elem_t *result = NULL;
  shelf_t *shelf = NULL;
  for (int i = 0; i <= siz; ++i)
    {
      list_get(master_list, i, result);
      shelf = elem_to_shelf(result);
      char *compare_shelf = shelf->shelf_name;
      if (compare_letter(shelf_name, compare_shelf) == 0)
        {
          return true;
        }
    }
  return false;
}

void key_exist(tree_t *tree, tree_key_t *key, char *shelf_name, list_t *master_list)
{  
   do
    {
      shelf_name = ask_question_shelf("Ange vilken hylla varan ligger på:");
      if (exist_shelf(master_list, shelf_name))
        {
          printf("Hyllan används redan. Ange en annan hylla.\n");
        }
    }
  while (exist_shelf(master_list, shelf_name));
   
  int amount = ask_question_price("Ange antal varor på hyllan:");
  
  elem_t *result = NULL;
  tree_get(tree, *key, result);
  item_t *change_item = elem_to_item(result);
  
  shelf_t *new_shelf = make_shelf(shelf_name, amount);
  elem_t *elem_new_shelf = shelf_to_elem(new_shelf);
  
  list_append(master_list, *elem_new_shelf);
  list_append(change_item->shelves, *elem_new_shelf);
}

tree_t *ask_question_new_item(tree_t *tree, list_t *master_list)
{

  tree_key_t *key = calloc(1, sizeof(tree_key_t));
  char *desc = calloc(1, sizeof(char));
  char *hylla = calloc(1, sizeof(char));
  shelf_t *shelf = calloc(1, sizeof(struct shelf));
  item_t *item = calloc(1, sizeof(struct item));
  list_t *shelves = list_new((element_copy_fun) copy_item,(element_free_fun) free,(element_comp_fun) compare_letter);

  char *name = calloc(1, sizeof(char));
  name = ask_question_string("Ange namn på varan:");

  key = char_to_elem(name);
  free(name);
  
  if (tree_has_key(tree, *key) == true)
    {
      char answer = ask_question_yes_no("Varan finns redan. Vill du lägga till den i en ny hylla? [J/N]");
      if (answer == 'J')
        {
          key_exist(tree, key, hylla, master_list);
          return tree;
        }
      else
        {
          return tree;
        }
    }

  desc = ask_question_string("Ange beskrivning av varan:");
  int price = ask_question_price("Ange varans pris:");
  do
    {
      hylla = ask_question_shelf("Ange vilken hylla varan ligger på:");
      if (exist_shelf(master_list, hylla))
        {
          printf("Hyllan används redan. Ange en annan hylla.\n");
        }
    }
  while (exist_shelf(master_list, hylla));
  int amount = ask_question_price("Ange antal varor på hyllan:");
  
  shelf = make_shelf(hylla, amount);
  elem_t *shelf_name = shelf_to_elem(shelf);
  list_append(master_list, *shelf_name);
  list_append(shelves, *shelf_name);
  
  item = make_item(desc, price, shelves);
  elem_t *elem_item = item_to_elem(item);
  
  tree_insert(tree, *key, *elem_item);
  
  return tree;
}

tree_key_t get_key(tree_t *tree, int pos)
{
  tree_key_t *key_list = tree_keys(tree);
  return key_list[pos];
}

int find_shelf_index(list_t *master_list, char *shelf_name)
{
  int len = list_length(master_list);
  elem_t *result = NULL;
  shelf_t *shelf = NULL;
  for (int i = 0; i <= len; ++i)
    {
      list_get(master_list, i, result);
      shelf = elem_to_shelf(result);
      char *compare_shelf = shelf->shelf_name;
      if (compare_letter(shelf_name, compare_shelf) == 0)
        {
          return i;
        }
    }
  return 0;
}


void remove_shelf(item_t *item, list_t *master_list, int index, struct action *savestate)
{ 
  bool delete = true;
  elem_t *result = NULL;

  edit_savestate(item, savestate, 2);
  
  list_remove(item->shelves, index, delete);
  
  list_get(item->shelves, index, result);
  shelf_t *shelf = elem_to_shelf(result);
  char *name = shelf->shelf_name;
  int index_master_list = find_shelf_index(master_list, name);
  list_remove(master_list, index_master_list, delete);
  
}

void free_item(item_t *item, struct action *savestate)
{
  edit_savestate(item, savestate, 2);
  
  free(item->desc);
  list_delete(item->shelves, true);
  
}

void delete_item_shelf(item_t *item, list_t *master_list, struct action *savestate)
{
  int len = list_length(item->shelves);
  int shelf = 0;
  int delete_num = 0;
  if (len > 1)
    {
      print_shelf(item);
      do
        {
          delete_num = ask_question_price("Ange vilken plats du vill ta bort:");
           if (shelf > len)
            {
              printf("Felaktig inmatning; skriv ett nummer i listan.\n");
            }
        }
      while (shelf > len);
      remove_shelf(item, master_list, delete_num, savestate);
    }
  else
    {
      free_item(item, savestate);
    }
}

tree_t delete_item(tree_t *tree, list_t *master_list, struct action *savestate)
{
  int page = tree_index(tree);
  int delete_num = 0;
  int size = tree_size(tree);

  do
    {
      delete_num = ask_question_price("Ange vilken vara du vill ta bort:");
    }
  while(delete_num < 1 || delete_num > size);
  delete_num = delete_num - 1 + (page * 20);

  tree_key_t key = get_key(tree, delete_num);
  char *key_name = elem_to_char(&key);
  elem_t *result = NULL;

  tree_get(tree, key, result);
  item_t *delete_item = elem_to_item(result);
  printf("%s finns på följade platser:\n", key_name);
  delete_item_shelf(delete_item, master_list, savestate);
  
  
}

void event_loop(tree_t *tree, list_t *master_list)
{
  bool quit = false;
  char input;
  struct action *savestate = calloc(1, sizeof(struct action));
  while (!quit)
    {
      input = ask_question_menu("\nVälkommen till lagerhantering 2.0\n=================================\n\n[L]ägg till en vara\n[T]a bort en vara\n[R]edigera en vara\nÅn[g]ra senaste ändringen\nLista [h]ela varukatalogen\n[S]ortera databasen\n[K]ontrollera databasens sortering\n[A]vsluta\n \nVad vill du göra idag? ");
      if ((input == 'L' || input == 'l'))
        {
          tree = ask_question_new_item(tree, master_list);
        }
      else if (input == 'T'|| input == 't')
        {
          tree = delete_item();
        }
      else if (input == 'R'|| input == 'r')
        {
          tree = edit_storage(tree, master_list, savestate);
        }
      else if (input == 'G'|| input == 'g')
        {
          undo_change(savestate);
        }
      else if (input == 'H'|| input == 'h')
        {
          tree_index(tree);
        }
       else if (input == 'S'|| input == 's')
        {
          tree = sort_tree();
        }
      else if (input == 'K'|| input == 'k')
        {
          is_tree_sorted();
        }
      else if (input == 'A'|| input == 'a')
        {
          quit = true;
        }
    }
  return;  
}
