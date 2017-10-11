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

struct shelf
{
  char *shelf;
  int amount;
};

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

elem_t make_items(char *desc, int price, list_t *shelves)
{
  elem_t *elem = calloc(1, sizeof(struct item));
  elem->desc = desc;
  elem->price = price;
  elem->shelves = shelves;
  return elem;
}

L make_shelves(char *shelf, int amount)
{
  L l = calloc(1, sizeof(struct shelf));
  l->shelf = shelf;
  l->amount = amount;
  return l;
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

list_t *copy_shelves(list_t *shelves)
{
  list_t *list_copy = list_new();
  int siz = list_length(shelves);

  for (int i = 0; i < siz; ++i)
    {
      struct shelf *tmp = *(list_get(shelves, i+1));
      struct shelf *shelf_copy = calloc(1, sizeof(struct shelf));
      
      shelf_copy->shelf = tmp->shelf;
      shelf_copy->amount = tmp->amount;
      
      list_append(list_copy, shelf_copy);
    }

  return list_copy;
}

struct item copy_item(T item)
{
  struct item *item_copy = calloc(1, sizeof(struct item));
  
  item_copy->desc = item->desc;
  item_copy->price = item->price;
  item_copy->shelves = copy_shelves(item->shelves);

  return *item_copy;
}

void edit_savestate(T item, struct action *savestate)
{  
  savestate->type = 2;
  savestate->copy = copy_item(item);
  savestate->orig = item;  
}


bool exist_shelf(list_t *master_list, char *shelf)
{
  int siz = list_length(master_list);
  for (int i = 0; i <= siz; ++i)
    {
      L *tmp = list_get(master_list, i);
      char *t = (*tmp)->shelf;
      if (compare_letter(shelf, t) == 0)
        {
          return true;
        }
    }
  return false;
}

void key_exist(tree_t *tree, K name, char *hylla, list_t *master_list)
{  
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
  T change_item = tree_get(tree, name);
  L new_shelf = make_shelves(hylla, amount);

  list_append(master_list, new_shelf);
  list_append(change_item->shelves, new_shelf);
}

tree_t *ask_question_new_item(tree_t *tree, list_t *master_list)
{

  K name = calloc(1, sizeof(char));
  char *desc = calloc(1, sizeof(char));
  char *hylla = calloc(1, sizeof(char));
  L shelf = calloc(1, sizeof(struct shelf));
  T item = calloc(1, sizeof(struct item));
  list_t *shelves = list_new();
  
  name = ask_question_string("Ange namn på varan:");
  if (tree_has_key(tree, name) == true)
    {
      char answer = ask_question_yes_no("Varan finns redan. Vill du lägga till den i en ny hylla? [J/N]");
      if (answer == 'J')
        {
          key_exist(tree, name, hylla, master_list);
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
  
  shelf = make_shelves(hylla, amount);
  list_append(master_list, shelf);
  list_append(shelves, shelf);
  item = make_items(desc, price, shelves);
  tree_insert(tree, name, item);
  return tree;
}

int tree_index(tree_t *tree)
{
  int screen_index = 1;
  K *keys = tree_keys(tree);
  int size = tree_size(tree);
  
  for (int i = 0; i < size; ++i, ++screen_index)
    {
      printf("%d. %s\n", screen_index, keys[i]);
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


void print_shelf_amount(T item)
{
  int size = list_length(item->shelves);
  for (int i = 1; i < size + 1; ++i)
    {
      L *shelf = list_get(item->shelves, i);
      char *name = (*shelf)->shelf;
      int amount = (*shelf)->amount;
      printf("Hyllplats: %s\n    Antal: %d\n", name, amount);
    }
}

void print_shelf_amount_index(T item)
{
  int size = list_length(item->shelves);
  for (int i = 1; i < size + 1; ++i)
    {
      L *shelf = list_get(item->shelves, i);
      char *name = (*shelf)->shelf;
      int amount = (*shelf)->amount;
      printf("%d. Hyllplats: %s\n       Antal: %d\n", i, name, amount);
    }
}


K print_node(tree_t *tree, int pos)
{
  K *keys = tree_keys(tree);
  T *items = tree_elements(tree);
  printf("Namn: %s\n", keys[pos]);
  printf("Beskriving: %s\n", items[pos]->desc);
  printf("Pris: %d kr\n", items[pos]->price);
  print_shelf_amount(items[pos]);  
  return keys[pos];
}


void edit_desc(tree_t *tree, K key, T item, struct action *savestate)
{
  char *newdesc = calloc(1, sizeof(char));
  T newnode = calloc(1, sizeof(struct item));
  
  printf("Nuvarande beskrivning: %s\n--------------------------------------------------------\n", item->desc);
  
  newdesc = ask_question_string("Ny beskrivning:");

  edit_savestate(item, savestate);
  
  newnode = tree_get(tree, key);
  newnode->desc = newdesc;
  tree_remove(tree, key);
  tree_insert(tree, key, newnode); 
}


void edit_price(tree_t *tree, K key, T item, struct action *savestate)
{
  T newnode = calloc(1, sizeof(struct item));

  printf("Nuvarande pris: %d kr\n--------------------------------------------------------\n", item->price);
  
  int newprice = ask_question_price("Nytt pris:");

  edit_savestate(item, savestate);
  
  newnode = tree_get(tree, key);
  newnode->price = newprice;
  tree_remove(tree, key);
  tree_insert(tree, key, newnode);
}


void print_shelf(T item)
{
  int size = list_length(item->shelves);
  for (int i = 1; i <= size; ++i)
    {
      L *shelf = list_get(item->shelves, i);
      char *name = (*shelf)->shelf;
      printf("%d. Hyllplats: %s\n", i, name);
    }
}


void replace_shelf(T item, int shelf, list_t *master_list, struct action *savestate)
{  
  char *newshelfname = calloc(1, sizeof(char));
  L *newshelf = calloc(1, sizeof(struct shelf));
  L *currentshelf = calloc(1, sizeof(struct shelf));
  currentshelf = list_get(item->shelves, shelf);
  
  printf("Nuvarande hylla: %s\n--------------------------------------------------------\n", (*currentshelf)->shelf);
  
  do
    {
      newshelfname = ask_question_shelf("Ny hylla:");
      if (exist_shelf(master_list, newshelfname))
        {
          printf("Hyllan används redan. Ange en annan hylla.\n ");
        }
    }
  while (exist_shelf(master_list, newshelfname));

  edit_savestate(item, savestate);
  
  newshelf = (L*) list_get(item->shelves, shelf);
  (*newshelf)->shelf = newshelfname;   
}

void edit_shelf(T item, list_t *master_list, struct action *savestate)
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
      
      replace_shelf(item, shelf, master_list, savestate);
    }
  else
    {
      replace_shelf(item, 0, master_list, savestate); 
    }

}


void replace_amount(T item, int shelf, struct action *savestate)
{  
  L *newshelf = calloc(1, sizeof(struct shelf));
  L *currentshelf = calloc(1, sizeof(struct shelf));
  currentshelf = list_get(item->shelves, shelf);
  
  printf("Nuvarande antal varor på hyllan: %d\n--------------------------------------------------------\n", (*currentshelf)->amount);
  
  int newamount = ask_question_price("Ntt antal:");

  edit_savestate(item, savestate);
  
  newshelf = (L*) list_get(item->shelves, shelf);
  (*newshelf)->amount = newamount;   
}


// funk för kopiera vara, returnerar kopian

void edit_amount(T item, struct action *savestate)
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
      
      replace_amount(item, shelf, savestate);
    }
  else
    {
      replace_amount(item, 0, savestate); 
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
  
  K key = print_node(tree, editnum);
  T item = tree_get(tree, key);
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
      edit_shelf(item, master_list, savestate);
      print_node(tree, editnum);
      return tree;
    }
  if ((input == 'T' || input == 't'))
    {
      edit_amount(item, savestate);
      print_node(tree, editnum);
      return tree;
    }
  if ((input == 'A' || input == 'a'))
    {
      return tree;
    }
  
return tree;
}

void undo_change(struct action *savestate)
{
  if (savestate->type == 0)
    {
      printf("Det finns inget att ångra!\n");
    }
  else if (savestate->type == 2)
    {
      *(savestate->orig) = savestate->copy;
      savestate->type = 0;
    }
}

void event_loop(tree_t *tree, list_t *master_list)
{
  bool quit = false;
  char input;
  struct action *savestate = calloc(1, sizeof(struct action));
  while (!quit)
    {
      input = ask_question_menu("\nVälkommen till lagerhantering 1.0\n=================================\n\n[L]ägg till en vara\n[T]a bort en vara\n[R]edigera en vara\nÅn[g]ra senaste ändringen\nLista [h]ela varukatalogen\n[A]vsluta\n \nVad vill du göra idag? ");
      if ((input == 'L' || input == 'l'))
        {
          tree = ask_question_new_item(tree, master_list);
        }
      else if (input == 'T'|| input == 't')
        {
          printf("Inte ännu implementerad\n");
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
      else if (input == 'A'|| input == 'a')
        {
          quit = true;
        }
    }
  return;  
}


int main()
{
  tree_t *tree = tree_new(); 
  list_t *master_list = list_new();

  list_t *shelf_for_item1 = list_new();
  L item1_shelf1 = make_shelves("A2", 4);
  L item1_shelf2 = make_shelves("K12", 9);
  list_append(shelf_for_item1, item1_shelf1);
  list_append(master_list, item1_shelf1);
  list_append(shelf_for_item1, item1_shelf2);
  list_append(master_list, item1_shelf2);
  T item1 = make_items("grön", 50, shelf_for_item1);

  list_t *shelf_for_item2 = list_new();
  L item2_shelf1 = make_shelves("L12", 14);
  L item2_shelf2 = make_shelves("S22", 4);
  list_append(shelf_for_item2, item2_shelf1);
  list_append(master_list, item2_shelf1);
  list_append(shelf_for_item2, item2_shelf2);
  list_append(master_list, item2_shelf2);
  T item2 = make_items("lättläst", 100, shelf_for_item2);
  
  list_t *shelf_for_item3 = list_new();
  L item3_shelf1 = make_shelves("D2", 21);
  list_append(shelf_for_item3, item3_shelf1);
  list_append(master_list, item3_shelf1);
  T item3 = make_items("självlysande", 5000, shelf_for_item3);

  list_t *shelf_for_item4 = list_new();
  L item4_shelf1 = make_shelves("W32", 34);
  L item4_shelf2 = make_shelves("A1", 4);
  list_append(shelf_for_item4, item4_shelf1);
  list_append(master_list, item4_shelf1);
  list_append(shelf_for_item4, item4_shelf2);
  list_append(master_list, item4_shelf2);
  T item4 = make_items("svart", 100, shelf_for_item4);

  list_t *shelf_for_item5 = list_new();
  L item5_shelf1 = make_shelves("H8", 34);
  L item5_shelf2 = make_shelves("F21", 4);
  list_append(shelf_for_item5, item5_shelf1);
  list_append(master_list, item5_shelf1);
  list_append(shelf_for_item5, item5_shelf2);
  list_append(master_list, item5_shelf2);
  T item5 = make_items("stor", 100, shelf_for_item5);

  list_t *shelf_for_item6 = list_new();
  L item6_shelf1 = make_shelves("N7", 34);
  L item6_shelf2 = make_shelves("S1", 4);
  list_append(shelf_for_item6, item6_shelf1);
  list_append(master_list, item6_shelf1);
  list_append(shelf_for_item6, item6_shelf2);
  list_append(master_list, item6_shelf2);
  T item6 = make_items("vit", 100, shelf_for_item6);

  list_t *shelf_for_item7 = list_new();
  L item7_shelf1 = make_shelves("G7", 34);
  L item7_shelf2 = make_shelves("A31", 4);
  list_append(shelf_for_item7, item7_shelf1);
  list_append(master_list, item7_shelf1);
  list_append(shelf_for_item7, item7_shelf2);
  list_append(master_list, item7_shelf2);
  T item7 = make_items("bläck", 100, shelf_for_item7);

  list_t *shelf_for_item8 = list_new();
  L item8_shelf1 = make_shelves("I9", 34);
  L item8_shelf2 = make_shelves("X21", 4);
  L item8_shelf3 = make_shelves("A44", 14);
  list_append(shelf_for_item8, item8_shelf1);
  list_append(shelf_for_item8, item8_shelf2);
  list_append(shelf_for_item8, item8_shelf3);
  list_append(master_list, item8_shelf1);
  list_append(master_list, item8_shelf2);
  list_append(master_list, item8_shelf3);
  T item8 = make_items("svarta", 100, shelf_for_item8);

  list_t *shelf_for_item9 = list_new();
  L item9_shelf1 = make_shelves("R7", 34);
  list_append(shelf_for_item9, item9_shelf1);
  list_append(master_list, item9_shelf1);
  T item9 = make_items("blå", 100, shelf_for_item9);

  list_t *shelf_for_item10 = list_new();
  L item10_shelf1 = make_shelves("G62", 34);
  L item10_shelf2 = make_shelves("G6", 4);
  list_append(shelf_for_item10, item10_shelf1);
  list_append(shelf_for_item10, item10_shelf2);
  list_append(master_list, item10_shelf1);
  list_append(master_list, item10_shelf2);
  T item10 = make_items("gul", 100, shelf_for_item10);

  list_t *shelf_for_item11 = list_new();
  L item11_shelf1 = make_shelves("G43", 34);
  L item11_shelf2 = make_shelves("Q1", 4);
  list_append(shelf_for_item11, item11_shelf1);
  list_append(shelf_for_item11, item11_shelf2);
  list_append(master_list, item11_shelf1);
   list_append(master_list, item11_shelf2);
  T item11 = make_items("blommig", 100, shelf_for_item11);

  list_t *shelf_for_item12 = list_new();
  L item12_shelf1 = make_shelves("H5", 34);
  L item12_shelf2 = make_shelves("G22", 4);
  list_append(shelf_for_item12, item12_shelf1);
  list_append(shelf_for_item12, item12_shelf2);
  list_append(master_list, item12_shelf1);
  list_append(master_list, item12_shelf2);
  T item12 = make_items("varma", 100, shelf_for_item12);

  list_t *shelf_for_item13 = list_new();
  L item13_shelf1 = make_shelves("K38", 34);
  L item13_shelf2 = make_shelves("P2", 4);
  list_append(shelf_for_item13, item13_shelf1);
  list_append(shelf_for_item13, item13_shelf2);
  list_append(master_list, item13_shelf1);
  list_append(master_list, item13_shelf2);
  T item13 = make_items("gosigt", 100, shelf_for_item13);

  list_t *shelf_for_item14 = list_new();
  L item14_shelf1 = make_shelves("S44", 34);
  L item14_shelf2 = make_shelves("Q6", 4);
  list_append(shelf_for_item14, item14_shelf1);
  list_append(shelf_for_item14, item14_shelf2);
  T item14 = make_items("blommönster", 100, shelf_for_item14);

  list_t *shelf_for_item15 = list_new();
  L item15_shelf1 = make_shelves("F4", 34);
  L item15_shelf2 = make_shelves("Z6", 4);
  list_append(shelf_for_item15, item15_shelf1);
  list_append(shelf_for_item15, item15_shelf2);
  list_append(master_list, item15_shelf1);
  list_append(master_list, item15_shelf2);
  T item15 = make_items("för torrt hår", 100, shelf_for_item15);
  
  list_t *shelf_for_item16 = list_new();
  L item16_shelf1 = make_shelves("H55", 34);
  L item16_shelf2 = make_shelves("E1", 4);
  list_append(shelf_for_item16, item16_shelf1);
  list_append(shelf_for_item16, item16_shelf2);
  list_append(master_list, item16_shelf1);
  list_append(master_list, item16_shelf2);
  T item16 = make_items("frotté", 100, shelf_for_item16);

  list_t *shelf_for_item17 = list_new();
  L item17_shelf1 = make_shelves("J7", 34);
  L item17_shelf2 = make_shelves("D44", 4);
  list_append(shelf_for_item17, item17_shelf1);
  list_append(shelf_for_item17, item17_shelf2);
  list_append(master_list, item17_shelf1);
  list_append(master_list, item17_shelf2);
  T item17 = make_items("snabb", 100, shelf_for_item17);
  
  list_t *shelf_for_item18 = list_new();
  L item18_shelf1 = make_shelves("R9", 34);
  L item18_shelf2 = make_shelves("I33", 4);
  list_append(shelf_for_item18, item18_shelf1);
  list_append(shelf_for_item18, item18_shelf2);
  list_append(master_list, item18_shelf1);
  list_append(master_list, item18_shelf2);
  T item18 = make_items("stor", 100, shelf_for_item18);

  list_t *shelf_for_item19 = list_new();
  L item19_shelf1 = make_shelves("K88", 34);
  L item19_shelf2 = make_shelves("F65", 4);
  list_append(shelf_for_item19, item19_shelf1);
  list_append(shelf_for_item19, item19_shelf2);
  list_append(master_list, item19_shelf1);
  list_append(master_list, item19_shelf2);
  T item19 = make_items("trä", 100, shelf_for_item19);
  
  list_t *shelf_for_item20 = list_new();
  L item20_shelf1 = make_shelves("S3", 34);
  L item20_shelf2 = make_shelves("A99", 4);
  list_append(shelf_for_item20, item20_shelf1);
  list_append(shelf_for_item20, item20_shelf2);
  T item20 = make_items("trä", 100, shelf_for_item20);
  
  list_t *shelf_for_item21 = list_new();
  L item21_shelf1 = make_shelves("A15", 34);
  L item21_shelf2 = make_shelves("A29", 4);
  list_append(shelf_for_item21, item21_shelf1);
  list_append(shelf_for_item21, item21_shelf2);
  list_append(master_list, item21_shelf1);
  list_append(master_list, item21_shelf2);
  T item21 = make_items("rostfri", 100, shelf_for_item21);

  list_t *shelf_for_item22 = list_new();
  L item22_shelf1 = make_shelves("O88", 34);
  L item22_shelf2 = make_shelves("U29", 4);
  list_append(shelf_for_item22, item22_shelf1);
  list_append(shelf_for_item22, item22_shelf2);
  list_append(master_list, item22_shelf1);
  list_append(master_list, item22_shelf2);
  T item22 = make_items("rostfri", 100, shelf_for_item22);
  

  tree_insert(tree, "stickad tröja", item1);
  tree_insert(tree, "barnbok", item2);
  tree_insert(tree, "keyboard", item3);
  tree_insert(tree, "plånbok", item4);
  tree_insert(tree, "handväska", item5);
  tree_insert(tree, "sudd", item6);
  tree_insert(tree, "penna", item7);
  tree_insert(tree, "strumpor", item8);
  tree_insert(tree, "jeans", item9);
  tree_insert(tree, "regnjacka", item10);
  tree_insert(tree, "pyjamas", item11);
  tree_insert(tree, "tofflor", item12);
  tree_insert(tree, "örngott", item13);
  tree_insert(tree, "toapapper", item14);
  tree_insert(tree, "schampoo", item15);
  tree_insert(tree, "handduk", item16);
  tree_insert(tree, "dator", item17);
  tree_insert(tree, "tv", item18);
  tree_insert(tree, "skrivbord", item19);
  tree_insert(tree, "matbord", item20);
  tree_insert(tree, "brödrost", item21);
  tree_insert(tree, "vattenkokare", item22);
  
  event_loop(tree, master_list);
  return 0;

}
