#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "utils.h"
#include "tree.h"
#include "list.h"
#include "common.h"
// #include "db.h"

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
  char *key;
  item_t copy;
  item_t *orig;
};

shelf_t *elem_to_shelf(elem_t pointer)
{
  return (shelf_t *)pointer.p;
}

elem_t shelf_to_elem(shelf_t *shelf)
{
  return (elem_t) { .p = shelf };
}

item_t *elem_to_item(elem_t pointer)
{
  return (item_t *)pointer.p;
}

elem_t item_to_elem(item_t *item)
{
  return (elem_t) { .p = item };
}

char *elem_to_char(elem_t pointer)
{
  return (char *)pointer.p;
}

elem_t char_to_elem(char *character)
{
  return (elem_t) { .p = character };
}


int compare_letter(elem_t first_elem, elem_t second_elem)
{
  char *first_string = elem_to_char(first_elem);
  char *second_string = elem_to_char(second_elem);
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

elem_t shelf_copy(elem_t elem)
{
  shelf_t *shelf_tmp  = elem_to_shelf(elem);
  shelf_t *shelf_copy = calloc(1, sizeof(shelf_t));
      
  shelf_copy->shelf_name = shelf_tmp->shelf_name;
  shelf_copy->amount = shelf_tmp->amount;

  elem_t elem_shelf_copy = shelf_to_elem(shelf_copy);
  return elem_shelf_copy;
}

void shelf_free(elem_t elem)
{
  shelf_t *shelf = elem_to_shelf(elem);
  free(shelf->shelf_name);
}

elem_t item_copy(elem_t elem)
{
  item_t *item = elem_to_item(elem);
  item_t *copy = calloc(1, sizeof(item_t));

  copy->desc = item->desc;
  copy->price = item->price;

  list_t *list_copy = list_new((element_copy_fun) shelf_copy, (element_free_fun) shelf_free, (element_comp_fun) compare_letter);
  int len = list_length(item->shelves);

  elem_t result;
  
  for (int i = 1; i <= len; ++i)
    {
     list_get(item->shelves, i, &result);
     list_append(list_copy, shelf_copy(result));
    }
  
  copy->shelves = list_copy;

  elem_t elem_item_copy = item_to_elem(copy);
  
  return elem_item_copy;
}

void key_free(tree_key_t key)
{
  elem_t *pointer = &key;
  free(pointer);
}

void item_free(elem_t *elem)
{
  item_t *item = elem_to_item(*elem);
  free(item->desc);
  list_delete(item->shelves, true);
}

void edit_savestate(item_t *item, tree_key_t elem_key, struct action *savestate, int type)
{  
  savestate->type = type;

  char *key = elem_to_char(elem_key);  
  savestate->key = key;
  
  elem_t elem = item_to_elem(item);
  elem = item_copy(elem);
  item_t *item_copy = elem_to_item(elem);
  savestate->copy = *item_copy;
  
  savestate->orig = item;  
}

shelf_t *make_shelf(char *shelf, int amount)
{
  shelf_t *new_shelf = calloc(1, sizeof(shelf_t));
  new_shelf->shelf_name = shelf;
  new_shelf->amount = amount;
  return new_shelf;
}

item_t *make_item(char *desc, int price, list_t *shelves)
{
  item_t *item = calloc(1, sizeof(item_t));
  item->desc = desc;
  item->price = price;
  item->shelves = shelves;
  return item;
}

void print_shelf(item_t *item)
{
  int size = list_length(item->shelves);
  elem_t result;
  shelf_t *shelf = NULL;
  for (int i = 1; i <= size; ++i)
    {
      list_get(item->shelves, i, &result);
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
      printf("%d. %s\n", screen_index, elem_to_char(keys[i]));
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
  elem_t result;
  shelf_t *shelf = NULL;
  for (int i = 0; i <= siz; ++i)
    {
      list_get(master_list, i, &result);
      shelf = elem_to_shelf(result);
      char *compare_shelf = shelf->shelf_name;
     
      if (compare_letter(char_to_elem(shelf_name), char_to_elem(compare_shelf)) == 0)
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
  
  elem_t result;
  tree_get(tree, *key, &result);
  item_t *change_item = elem_to_item(result);
  
  shelf_t *new_shelf = make_shelf(shelf_name, amount);
  elem_t elem_new_shelf = shelf_to_elem(new_shelf);
  
  list_append(master_list, elem_new_shelf);
  list_append(change_item->shelves, elem_new_shelf);
}

tree_t *ask_question_new_item(tree_t *tree, list_t *master_list)
{

  tree_key_t *key = calloc(1, sizeof(tree_key_t));
  char *desc = calloc(1, sizeof(char));
  char *hylla = calloc(1, sizeof(char));
  shelf_t *shelf = calloc(1, sizeof(struct shelf));
  item_t *item = calloc(1, sizeof(struct item));
  list_t *shelves = list_new((element_copy_fun) shelf_copy,(element_free_fun) free,(element_comp_fun) compare_letter);

  char *name = calloc(1, sizeof(char));
  name = ask_question_string("Ange namn på varan:");

  *key = char_to_elem(name);
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
  elem_t shelf_name = shelf_to_elem(shelf);
  list_append(master_list, shelf_name);
  list_append(shelves, shelf_name);
  
  item = make_item(desc, price, shelves);
  elem_t elem_item = item_to_elem(item);
  
  tree_insert(tree, *key, elem_item);
  
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
  elem_t result;
  shelf_t *shelf = NULL;
  for (int i = 0; i <= len; ++i)
    {
      list_get(master_list, i, &result);
      shelf = elem_to_shelf(result);
      char *compare_shelf = shelf->shelf_name;
      if (compare_letter(char_to_elem(shelf_name), char_to_elem(compare_shelf)) == 0)
        {
          return i;
        }
    }
  return 0;
}


void remove_shelf(item_t *item, tree_key_t key, list_t *master_list, int index, struct action *savestate)
{ 
  bool delete = true;
  elem_t result;

  edit_savestate(item, key, savestate, 2);
  
  list_remove(item->shelves, index, delete);
  
  list_get(item->shelves, index, &result);
  shelf_t *shelf = elem_to_shelf(result);
  char *name = shelf->shelf_name;
  int index_master_list = find_shelf_index(master_list, name);
  list_remove(master_list, index_master_list, delete);
  
}

void free_item(item_t *item, tree_key_t key, struct action *savestate)
{
  edit_savestate(item, key, savestate, 2);
  
  free(item->desc);
  list_delete(item->shelves, true);
  
}

void delete_item_shelf(item_t *item, tree_key_t key, list_t *master_list, struct action *savestate)
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
      remove_shelf(item, key, master_list, delete_num, savestate);
    }
  else
    {
      free_item(item, key, savestate);
    }
}

tree_t *delete_item(tree_t *tree, list_t *master_list, struct action *savestate)
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
  char *key_name = elem_to_char(key);
  elem_t result;

  tree_get(tree, key, &result);
  item_t *delete_item = elem_to_item(result);
  printf("%s finns på följade platser:\n", key_name);
  delete_item_shelf(delete_item, key, master_list, savestate);

  return tree;
}

void print_shelf_amount(item_t *item)
{
  int size = list_length(item->shelves);
  elem_t result;
  for (int i = 1; i < size + 1; ++i)
    {
      list_get(item->shelves, i, &result);
      shelf_t *shelf = elem_to_shelf(result);
      char *name = shelf->shelf_name;
      int amount = shelf->amount;
      printf("Hyllplats: %s\n    Antal: %d\n", name, amount);
    }
}

void print_shelf_amount_index(item_t *item)
{
  int size = list_length(item->shelves);
  elem_t result;
  for (int i = 1; i < size + 1; ++i)
    {
      list_get(item->shelves, i, &result);
      shelf_t *shelf = elem_to_shelf(result); 
      char *name = shelf->shelf_name;
      int amount = shelf->amount;
      printf("%d. Hyllplats: %s\n       Antal: %d\n", i, name, amount);
    }
}


tree_key_t print_node(tree_t *tree, int pos)
{
  tree_key_t *keys = tree_keys(tree);
  
  elem_t *elem_items = tree_elements(tree);
  item_t *items = elem_to_item(*elem_items);
  
  printf("Namn: %s\n", elem_to_char(keys[pos]));
  printf("Beskriving: %s\n", items[pos].desc);
  printf("Pris: %d kr\n", items[pos].price);
  print_shelf_amount(&items[pos]);  
  return keys[pos];
}


void edit_desc(tree_t *tree, tree_key_t key, item_t *item, struct action *savestate)
{
  char *new_desc = calloc(1, sizeof(char));
  item_t *new_node = calloc(1, sizeof(item_t));
  
  printf("Nuvarande beskrivning: %s\n--------------------------------------------------------\n", item->desc);
  
  new_desc = ask_question_string("Ny beskrivning:");

  edit_savestate(item, key, savestate, 3);

  elem_t result1;
  tree_get(tree, key, &result1);
  
  new_node = elem_to_item(result1);
  new_node->desc = new_desc;

  elem_t result2;
  tree_remove(tree, key, &result2);

  elem_t elem_new_node = item_to_elem(new_node);
  tree_insert(tree, key, elem_new_node); 
}

void edit_price(tree_t *tree, tree_key_t key, item_t *item, struct action *savestate)
{
  item_t *new_node = calloc(1, sizeof(item_t));

  printf("Nuvarande pris: %d kr\n--------------------------------------------------------\n", item->price);
  
  int new_price = ask_question_price("Nytt pris:");

  edit_savestate(item, key, savestate, 3);

  elem_t result1;
  tree_get(tree, key, &result1);

  new_node = elem_to_item(result1);
  new_node->price = new_price;

  elem_t result2;
  tree_remove(tree, key, &result2);

  elem_t elem_new_node = item_to_elem(new_node);
  tree_insert(tree, key, elem_new_node);
}

void replace_shelf(item_t *item, tree_key_t key, int shelf, list_t *master_list, struct action *savestate)
{  
  char *new_shelf_name = calloc(1, sizeof(char));
  shelf_t *new_shelf = calloc(1, sizeof(shelf_t));
  shelf_t *current_shelf = calloc(1, sizeof(shelf_t));
  elem_t result1;
  list_get(item->shelves, shelf, &result1);
  current_shelf = elem_to_shelf(result1);
  
  printf("Nuvarande hylla: %s\n--------------------------------------------------------\n", current_shelf->shelf_name);
  
  do
    {
      new_shelf_name = ask_question_shelf("Ny hylla:");
      if (exist_shelf(master_list, new_shelf_name))
        {
          printf("Hyllan används redan. Ange en annan hylla.\n ");
        }
    }
  while (exist_shelf(master_list, new_shelf_name));

  edit_savestate(item, key, savestate, 3);

  elem_t result2;
  list_get(item->shelves, shelf, &result2);
  new_shelf = elem_to_shelf(result2);
  new_shelf->shelf_name = new_shelf_name;   
}

void edit_shelf(item_t *item, tree_key_t key, list_t *master_list, struct action *savestate)
{
  int size = list_length(item->shelves);
  int shelf = 0;
  if (size > 1)
    {
      print_shelf(item);
      do
        {
          shelf = ask_question_price("Ange vilken hylla du vill ändra:");
          if (shelf > size)
            {
              printf("Felaktig inmatning; skriv ett nummer i listan.\n");
            }
        }
      while (shelf > size);
      
      replace_shelf(item, key, shelf, master_list, savestate);
    }
  else
    {
      replace_shelf(item, key, 0, master_list, savestate); 
    }

}

void replace_amount(item_t *item, tree_key_t key, int shelf, struct action *savestate)
{  
  shelf_t *new_shelf = calloc(1, sizeof(shelf_t));
  shelf_t *current_shelf = calloc(1, sizeof(shelf_t));

  elem_t result1;
  list_get(item->shelves, shelf, &result1);
  current_shelf = elem_to_shelf(result1);
  
  printf("Nuvarande antal varor på hyllan: %d\n--------------------------------------------------------\n", current_shelf->amount);
  
  int new_amount = ask_question_price("Ntt antal:");

  edit_savestate(item, key, savestate, 3);
  
  elem_t result2;
  list_get(item->shelves, shelf, &result2);
  new_shelf = elem_to_shelf(result2);
  new_shelf->amount = new_amount;   
}

void edit_amount(item_t *item, tree_key_t key, struct action *savestate)
{
  int size = list_length(item->shelves);
  int shelf = 0;
  if (size > 1)
    {
      print_shelf_amount_index(item);
      do
        {
          shelf = ask_question_price("Ange vilken hylla du vill ändra antalet varor på:");
          if (shelf > size)
            {
              printf("Felaktig inmatning; skriv ett nummer i listan");
            }
        }
      while (shelf > size);
      
      replace_amount(item, key, shelf, savestate);
    }
  else
    {
      replace_amount(item, key, 0, savestate); 
    }

}


tree_t *edit_storage(tree_t *tree, list_t *master_list, struct action *savestate)
{
  int page = tree_index(tree);
  int editnum = 0;
  int size = tree_size(tree);
  
  do
    {
      editnum = ask_question_price("Ange vilken vara du ändra:");
      if (editnum > size)
        {
          printf("Felaktig inmatning, skriv ett nummer på en vara\n");
        }
    }
  while(editnum < 1 || editnum > size);
  editnum = editnum - 1 + (page * 20);
  
  tree_key_t key = print_node(tree, editnum);

  elem_t result;
  tree_get(tree, key, &result);

  item_t *item = elem_to_item(result);
  
  char input = ask_question_edit("\n[B]eskrivning\n[P]ris\n[L]agerhylla\nAn[t]al\n\nVälj rad eller [a]vbryt:");
  if ((input == 'B' || input == 'b'))
    {
      edit_desc(tree, key, item, savestate);
      print_node(tree, editnum);
      return tree;
    }
  if ((input == 'P' || input == 'p'))
    {
      edit_price(tree, key, item, savestate);
      print_node(tree, editnum);
      return tree;
    }
  if ((input == 'L' || input == 'l'))
    {
      edit_shelf(item, key, master_list, savestate);
      print_node(tree, editnum);
      return tree;
    }
  if ((input == 'T' || input == 't'))
    {
      edit_amount(item, key, savestate);
      print_node(tree, editnum);
      return tree;
    }
  if ((input == 'A' || input == 'a'))
    {
      return tree;
    }
  
return tree;
}

void undo_change(tree_t *tree, struct action *savestate)
{
  if (savestate->type == 0)
    {
      printf("Det finns inget att ångra!\n");
    }
  else if (savestate->type == 1)
    {
      elem_t elem_orig = item_to_elem(savestate->orig);
      item_free(&elem_orig);

      elem_t key = char_to_elem(savestate->key);
      key_free(key);
      
      savestate->type = 0;
    }
  else if (savestate->type == 2)
    {
      //*(savestate->orig) = savestate->copy;
      
      item_t *item = savestate->orig;
      elem_t elem_item = item_to_elem(item);

      tree_key_t key = char_to_elem(savestate->key);
      
      tree_insert(tree, key, elem_item);
      savestate->type = 0;
    }
  else if (savestate->type == 3)
    {
      *(savestate->orig) = savestate->copy;
      savestate->type = 0;
    }
}

void is_tree_sorted(tree_t *tree)
{
  int round_up = 0;
  float sqrt_tree = sqrt(tree_size(tree) + 1);
  if (round(sqrt_tree) < sqrt_tree)
    {
      round_up = 1;
    }
  if (tree_depth(tree) > round(sqrt_tree) + round_up)
    {
      printf("Databasen är ej sorterad\n");
    }
  else
    {
      printf("Databasen är sorterad!\n");
    }
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
          tree = delete_item(tree, master_list, savestate);
        }
      else if (input == 'R'|| input == 'r')
        {
          tree = edit_storage(tree, master_list, savestate);
        }
      else if (input == 'G'|| input == 'g')
        {
          undo_change(tree, savestate);
        }
      else if (input == 'H'|| input == 'h')
        {
          tree_index(tree);
        }
       else if (input == 'S'|| input == 's')
        {
          tree_sort(tree);
          savestate->type = 0;
        }
      else if (input == 'K'|| input == 'k')
        {
          is_tree_sorted(tree);
        }
      else if (input == 'A'|| input == 'a')
        {
          quit = true;
        }
    }
  return;  
}


int main()
{
  tree_t *tree = tree_new((element_copy_fun) item_copy, (key_free_fun) key_free, (element_free_fun) item_free, (element_comp_fun) compare_letter);
  list_t *master_list = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);

  list_t *shelf_for_item1 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item1_shelf1 = make_shelf("A2", 4);
  shelf_t *item1_shelf2 = make_shelf("K12", 9);
  elem_t elem_item1_shelf1 = shelf_to_elem(item1_shelf1);
  elem_t elem_item1_shelf2 = shelf_to_elem(item1_shelf2);
  list_append(shelf_for_item1, elem_item1_shelf1);
  list_append(master_list, elem_item1_shelf1);
  list_append(shelf_for_item1, elem_item1_shelf2);
  list_append(master_list, elem_item1_shelf2);
  item_t *item1 = make_item("grön", 50, shelf_for_item1);
  elem_t elem_item1 = item_to_elem(item1);

  list_t *shelf_for_item2 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item2_shelf1 = make_shelf("L12", 14);
  shelf_t *item2_shelf2 = make_shelf("S22", 4);
  elem_t elem_item2_shelf1 = shelf_to_elem(item2_shelf1);
  elem_t elem_item2_shelf2 = shelf_to_elem(item2_shelf2);
  list_append(shelf_for_item2, elem_item2_shelf1);
  list_append(master_list, elem_item2_shelf1);
  list_append(shelf_for_item2, elem_item2_shelf2);
  list_append(master_list, elem_item2_shelf2);
  item_t *item2 = make_item("lättläst", 100, shelf_for_item2);
  elem_t elem_item2 = item_to_elem(item2);
  
  list_t *shelf_for_item3 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item3_shelf1 = make_shelf("D2", 21);
  elem_t elem_item3_shelf1 = shelf_to_elem(item3_shelf1);
  list_append(shelf_for_item3, elem_item3_shelf1);
  list_append(master_list, elem_item3_shelf1);
  item_t *item3 = make_item("självlysande", 5000, shelf_for_item3);
  elem_t elem_item3 = item_to_elem(item3);

  list_t *shelf_for_item4 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item4_shelf1 = make_shelf("W32", 34);
  shelf_t *item4_shelf2 = make_shelf("A1", 4);
  elem_t elem_item4_shelf1 = shelf_to_elem(item4_shelf1);
  elem_t elem_item4_shelf2 = shelf_to_elem(item4_shelf2);
  list_append(shelf_for_item4, elem_item4_shelf1);
  list_append(master_list, elem_item4_shelf1);
  list_append(shelf_for_item4, elem_item4_shelf2);
  list_append(master_list, elem_item4_shelf2);
  item_t *item4 = make_item("svart", 100, shelf_for_item4);
  elem_t elem_item4 = item_to_elem(item4);

  list_t *shelf_for_item5 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item5_shelf1 = make_shelf("H8", 34);
  shelf_t *item5_shelf2 = make_shelf("F21", 4);
  elem_t elem_item5_shelf1 = shelf_to_elem(item5_shelf1);
  elem_t elem_item5_shelf2 = shelf_to_elem(item5_shelf2);
  list_append(shelf_for_item5, elem_item5_shelf1);
  list_append(master_list, elem_item5_shelf1);
  list_append(shelf_for_item5, elem_item5_shelf2);
  list_append(master_list, elem_item5_shelf2);
  item_t *item5 = make_item("stor", 100, shelf_for_item5);
  elem_t elem_item5 = item_to_elem(item5);

  list_t *shelf_for_item6 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item6_shelf1 = make_shelf("N7", 34);
  shelf_t *item6_shelf2 = make_shelf("S1", 4);
  elem_t elem_item6_shelf1 = shelf_to_elem(item6_shelf1);
  elem_t elem_item6_shelf2 = shelf_to_elem(item6_shelf2);
  list_append(shelf_for_item6, elem_item6_shelf1);
  list_append(master_list, elem_item6_shelf1);
  list_append(shelf_for_item6, elem_item6_shelf2);
  list_append(master_list, elem_item6_shelf2);
  item_t *item6 = make_item("vit", 100, shelf_for_item6);
  elem_t elem_item6 = item_to_elem(item6);

  list_t *shelf_for_item7 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item7_shelf1 = make_shelf("G7", 34);
  shelf_t *item7_shelf2 = make_shelf("A31", 4);
  elem_t elem_item7_shelf1 = shelf_to_elem(item7_shelf1);
  elem_t elem_item7_shelf2 = shelf_to_elem(item7_shelf2);
  list_append(shelf_for_item7, elem_item7_shelf1);
  list_append(master_list, elem_item7_shelf1);
  list_append(shelf_for_item7, elem_item7_shelf2);
  list_append(master_list, elem_item7_shelf2);
  item_t *item7 = make_item("bläck", 100, shelf_for_item7);
  elem_t elem_item7 = item_to_elem(item7);

  list_t *shelf_for_item8 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item8_shelf1 = make_shelf("I9", 34);
  shelf_t *item8_shelf2 = make_shelf("X21", 4);
  shelf_t *item8_shelf3 = make_shelf("A44", 14);
  elem_t elem_item8_shelf1 = shelf_to_elem(item8_shelf1);
  elem_t elem_item8_shelf2 = shelf_to_elem(item8_shelf2);
  elem_t elem_item8_shelf3 = shelf_to_elem(item8_shelf3);
  list_append(shelf_for_item8, elem_item8_shelf1);
  list_append(shelf_for_item8, elem_item8_shelf2);
  list_append(shelf_for_item8, elem_item8_shelf3);
  list_append(master_list, elem_item8_shelf1);
  list_append(master_list, elem_item8_shelf2);
  list_append(master_list, elem_item8_shelf3);
  item_t *item8 = make_item("svarta", 100, shelf_for_item8);
  elem_t elem_item8 = item_to_elem(item8);

  list_t *shelf_for_item9 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item9_shelf1 = make_shelf("R7", 34);
  elem_t elem_item9_shelf1 = shelf_to_elem(item9_shelf1);
  list_append(shelf_for_item9, elem_item9_shelf1);
  list_append(master_list, elem_item9_shelf1);
  item_t *item9 = make_item("blå", 100, shelf_for_item9);
  elem_t elem_item9 = item_to_elem(item9);

  list_t *shelf_for_item10 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item10_shelf1 = make_shelf("G62", 34);
  shelf_t *item10_shelf2 = make_shelf("G6", 4);
  elem_t elem_item10_shelf1 = shelf_to_elem(item10_shelf1);
  elem_t elem_item10_shelf2 = shelf_to_elem(item10_shelf2);
  list_append(shelf_for_item10, elem_item10_shelf1);
  list_append(shelf_for_item10, elem_item10_shelf2);
  list_append(master_list, elem_item10_shelf1);
  list_append(master_list, elem_item10_shelf2);
  item_t *item10 = make_item("gul", 100, shelf_for_item10);
  elem_t elem_item10 = item_to_elem(item10);

  list_t *shelf_for_item11 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item11_shelf1 = make_shelf("G43", 34);
  shelf_t *item11_shelf2 = make_shelf("Q1", 4);
  elem_t elem_item11_shelf1 = shelf_to_elem(item11_shelf1);
  elem_t elem_item11_shelf2 = shelf_to_elem(item11_shelf2);
  list_append(shelf_for_item11, elem_item11_shelf1);
  list_append(shelf_for_item11, elem_item11_shelf2);
  list_append(master_list, elem_item11_shelf1);
  list_append(master_list, elem_item11_shelf2);
  item_t *item11 = make_item("blommig", 100, shelf_for_item11);
  elem_t elem_item11 = item_to_elem(item11);

  list_t *shelf_for_item12 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item12_shelf1 = make_shelf("H5", 34);
  shelf_t *item12_shelf2 = make_shelf("G22", 4);
  elem_t elem_item12_shelf1 = shelf_to_elem(item12_shelf1);
  elem_t elem_item12_shelf2 = shelf_to_elem(item12_shelf2);
  list_append(shelf_for_item12, elem_item12_shelf1);
  list_append(shelf_for_item12, elem_item12_shelf2);
  list_append(master_list, elem_item12_shelf1);
  list_append(master_list, elem_item12_shelf2);
  item_t *item12 = make_item("varma", 100, shelf_for_item12);
  elem_t elem_item12 = item_to_elem(item12);

  list_t *shelf_for_item13 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item13_shelf1 = make_shelf("K38", 34);
  shelf_t *item13_shelf2 = make_shelf("P2", 4);
  elem_t elem_item13_shelf1 = shelf_to_elem(item13_shelf1);
  elem_t elem_item13_shelf2 = shelf_to_elem(item13_shelf2);
  list_append(shelf_for_item13, elem_item13_shelf1);
  list_append(shelf_for_item13, elem_item13_shelf2);
  list_append(master_list, elem_item13_shelf1);
  list_append(master_list, elem_item13_shelf2);
  item_t *item13 = make_item("gosigt", 100, shelf_for_item13);
  elem_t elem_item13 = item_to_elem(item13);

  list_t *shelf_for_item14 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item14_shelf1 = make_shelf("S44", 34);
  shelf_t *item14_shelf2 = make_shelf("Q6", 4);
  elem_t elem_item14_shelf1 = shelf_to_elem(item14_shelf1);
  elem_t elem_item14_shelf2 = shelf_to_elem(item14_shelf2);
  list_append(shelf_for_item14, elem_item14_shelf1);
  list_append(shelf_for_item14, elem_item14_shelf2);
  item_t *item14 = make_item("blommönster", 100, shelf_for_item14);
  elem_t elem_item14 = item_to_elem(item14);

  list_t *shelf_for_item15 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item15_shelf1 = make_shelf("F4", 34);
  shelf_t *item15_shelf2 = make_shelf("Z6", 4);
  elem_t elem_item15_shelf1 = shelf_to_elem(item15_shelf1);
  elem_t elem_item15_shelf2 = shelf_to_elem(item15_shelf2);
  list_append(shelf_for_item15, elem_item15_shelf1);
  list_append(shelf_for_item15, elem_item15_shelf2);
  list_append(master_list, elem_item15_shelf1);
  list_append(master_list, elem_item15_shelf2);
  item_t *item15 = make_item("för torrt hår", 100, shelf_for_item15);
  elem_t elem_item15 = item_to_elem(item15);
  
  list_t *shelf_for_item16 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item16_shelf1 = make_shelf("H55", 34);
  shelf_t *item16_shelf2 = make_shelf("E1", 4);
  elem_t elem_item16_shelf1 = shelf_to_elem(item16_shelf1);
  elem_t elem_item16_shelf2 = shelf_to_elem(item16_shelf2);
  list_append(shelf_for_item16, elem_item16_shelf1);
  list_append(shelf_for_item16, elem_item16_shelf2);
  list_append(master_list, elem_item16_shelf1);
  list_append(master_list, elem_item16_shelf2);
  item_t *item16 = make_item("frotté", 100, shelf_for_item16);
  elem_t elem_item16 = item_to_elem(item16);

  list_t *shelf_for_item17 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item17_shelf1 = make_shelf("J7", 34);
  shelf_t *item17_shelf2 = make_shelf("D44", 4);
  elem_t elem_item17_shelf1 = shelf_to_elem(item17_shelf1);
  elem_t elem_item17_shelf2 = shelf_to_elem(item17_shelf2);
  list_append(shelf_for_item17, elem_item17_shelf1);
  list_append(shelf_for_item17, elem_item17_shelf2);
  list_append(master_list, elem_item17_shelf1);
  list_append(master_list, elem_item17_shelf2);
  item_t *item17 = make_item("snabb", 100, shelf_for_item17);
  elem_t elem_item17 = item_to_elem(item17);
  
  list_t *shelf_for_item18 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item18_shelf1 = make_shelf("R9", 34);
  shelf_t *item18_shelf2 = make_shelf("I33", 4);
  elem_t elem_item18_shelf1 = shelf_to_elem(item18_shelf1);
  elem_t elem_item18_shelf2 = shelf_to_elem(item18_shelf2);
  list_append(shelf_for_item18, elem_item18_shelf1);
  list_append(shelf_for_item18, elem_item18_shelf2);
  list_append(master_list, elem_item18_shelf1);
  list_append(master_list, elem_item18_shelf2);
  item_t *item18 = make_item("stor", 100, shelf_for_item18);
  elem_t elem_item18 = item_to_elem(item18);

  list_t *shelf_for_item19 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item19_shelf1 = make_shelf("K88", 34);
  shelf_t *item19_shelf2 = make_shelf("F65", 4);
  elem_t elem_item19_shelf1 = shelf_to_elem(item19_shelf1);
  elem_t elem_item19_shelf2 = shelf_to_elem(item19_shelf2);
  list_append(shelf_for_item19, elem_item19_shelf1);
  list_append(shelf_for_item19, elem_item19_shelf2);
  list_append(master_list, elem_item19_shelf1);
  list_append(master_list, elem_item19_shelf2);
  item_t *item19 = make_item("trä", 100, shelf_for_item19);
  elem_t elem_item19 = item_to_elem(item19);
  
  list_t *shelf_for_item20 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item20_shelf1 = make_shelf("S3", 34);
  shelf_t *item20_shelf2 = make_shelf("A99", 4);
  elem_t elem_item20_shelf1 = shelf_to_elem(item20_shelf1);
  elem_t elem_item20_shelf2 = shelf_to_elem(item20_shelf2);
  list_append(shelf_for_item20, elem_item20_shelf1);
  list_append(shelf_for_item20, elem_item20_shelf2);
  list_append(master_list, elem_item20_shelf1);
  list_append(master_list, elem_item20_shelf2);
  item_t *item20 = make_item("trä", 100, shelf_for_item20);
  elem_t elem_item20 = item_to_elem(item20);
  
  list_t *shelf_for_item21 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item21_shelf1 = make_shelf("A15", 34);
  shelf_t *item21_shelf2 = make_shelf("A29", 4);
  elem_t elem_item21_shelf1 = shelf_to_elem(item21_shelf1);
  elem_t elem_item21_shelf2 = shelf_to_elem(item21_shelf2);
  list_append(shelf_for_item21, elem_item21_shelf1);
  list_append(shelf_for_item21, elem_item21_shelf2);
  list_append(master_list, elem_item21_shelf1);
  list_append(master_list, elem_item21_shelf2);
  item_t *item21 = make_item("rostfri", 100, shelf_for_item21);
  elem_t elem_item21 = item_to_elem(item21);

  list_t *shelf_for_item22 = list_new((element_copy_fun) shelf_copy,(element_free_fun) shelf_free,(element_comp_fun) compare_letter);
  shelf_t *item22_shelf1 = make_shelf("O88", 34);
  shelf_t *item22_shelf2 = make_shelf("U29", 4);
  elem_t elem_item22_shelf1 = shelf_to_elem(item22_shelf1);
  elem_t elem_item22_shelf2 = shelf_to_elem(item22_shelf2);
  list_append(shelf_for_item22, elem_item22_shelf1);
  list_append(shelf_for_item22, elem_item22_shelf2);
  list_append(master_list, elem_item22_shelf1);
  list_append(master_list, elem_item22_shelf2);
  item_t *item22 = make_item("rostfri", 100, shelf_for_item22);
  elem_t elem_item22 = item_to_elem(item22);
  

  char *key1 = "sickad tröja";
  elem_t elem_key1 = char_to_elem(key1);

  char *key2 = "barnbok";
  elem_t elem_key2 = char_to_elem(key2);

  char *key3 = "keyboard";
  elem_t elem_key3 = char_to_elem(key3);

  char *key4 = "plånbok";
  elem_t elem_key4 = char_to_elem(key4);

  char *key5 = "handväska";
  elem_t elem_key5 = char_to_elem(key5);

  char *key6 = "sudd";
  elem_t elem_key6 = char_to_elem(key6);

  char *key7 = "penna";
  elem_t elem_key7 = char_to_elem(key7);

  char *key8 = "strumpor"; // skrivs ej
  elem_t elem_key8 = char_to_elem(key8);

  char *key9 = "jeans"; // skrivs ej
  elem_t elem_key9 = char_to_elem(key9);

  char *key10 = "regnjacka"; // skrivs ej 
  elem_t elem_key10 = char_to_elem(key10);

  char *key11 = "pyjamas";
  elem_t elem_key11 = char_to_elem(key11);

  char *key12 = "tofflor";
  elem_t elem_key12 = char_to_elem(key12);

  char *key13 = "örngott";
  elem_t elem_key13 = char_to_elem(key13);

  char *key14 = "toapapper";
  elem_t elem_key14 = char_to_elem(key14);

  char *key15 = "schampoo";
  elem_t elem_key15 = char_to_elem(key15);

  char *key16 = "handduk";
  elem_t elem_key16 = char_to_elem(key16);

  char *key17 = "dator";
  elem_t elem_key17 = char_to_elem(key17);

  char *key18 = "tv";
  elem_t elem_key18 = char_to_elem(key18);

  char *key19 = "skrivbord";
  elem_t elem_key19 = char_to_elem(key19);

  char *key20 = "matbord";
  elem_t elem_key20 = char_to_elem(key20);

  char *key21 = "toaster";
  elem_t elem_key21 = char_to_elem(key21);

  char *key22 = "vattenkokare";
  elem_t elem_key22 = char_to_elem(key22);
  
  tree_insert(tree, elem_key1, elem_item1);
  tree_insert(tree, elem_key2, elem_item2);
  tree_insert(tree, elem_key3, elem_item3);
  tree_insert(tree, elem_key4, elem_item4);
  tree_insert(tree, elem_key5, elem_item5);
  tree_insert(tree, elem_key6, elem_item6);
  tree_insert(tree, elem_key7, elem_item7);
  tree_insert(tree, elem_key8, elem_item8);
  tree_insert(tree, elem_key9, elem_item9);
  tree_insert(tree, elem_key10, elem_item10);
  tree_insert(tree, elem_key11, elem_item11);
  tree_insert(tree, elem_key12, elem_item12);
  tree_insert(tree, elem_key13, elem_item13);
  tree_insert(tree, elem_key14, elem_item14);
  tree_insert(tree, elem_key15, elem_item15);
  tree_insert(tree, elem_key16, elem_item16);
  tree_insert(tree, elem_key17, elem_item17);
  tree_insert(tree, elem_key18, elem_item18);
  tree_insert(tree, elem_key19, elem_item19);
  tree_insert(tree, elem_key20, elem_item20);
  tree_insert(tree, elem_key21, elem_item21);
  tree_insert(tree, elem_key22, elem_item22);
  
  event_loop(tree, master_list);
  return 0;

}
