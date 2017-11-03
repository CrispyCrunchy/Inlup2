#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "tree.h"
#include "common.h"

typedef struct node node_t;

struct node
{
  tree_key_t key;
  elem_t elem;
    
  node_t *left;
  node_t *right;
};

struct tree
{
  node_t *root;
  
  key_free_fun key_free;
  element_free_fun elem_free;
  element_comp_fun comp;
  element_copy_fun copy;
};

tree_t *tree_new(element_copy_fun elem_copy, key_free_fun key_free, element_free_fun elem_free, element_comp_fun comp)
{
  tree_t *new = calloc(1, sizeof(tree_t));
  new->copy = elem_copy;
  new->key_free = key_free;
  new->elem_free = elem_free;
  new->comp = comp;
  return new;
}

void node_delete(tree_t *tree, node_t *node, bool delete_keys, bool delete_elements)
{
  if (node)
    {
      if (node->left)
        {
          node_delete(tree, node->left, delete_keys, delete_elements);
        }
      if (node->right)
        {
          node_delete(tree, node->right, delete_keys, delete_elements);
        }
      if (delete_keys)
        {
          if (node->key.p != NULL)
            {
              tree->key_free(node->key);
            }
        }
      if (delete_elements)
        {
          if (node->elem.p != NULL)
            {
              tree->elem_free(node->elem);
            }
        }
      free(node);
    }
}


void tree_delete(tree_t *tree, bool delete_keys, bool delete_elements)
{
  node_delete(tree, tree->root, delete_keys, delete_elements);
  free(tree);
}


int node_size(node_t *node)
{
  if (node)
    {
      return node_size(node->left) + node_size(node->right) + 1;
    }
  else
    {
      return 0;
    }
}
  
int tree_size(tree_t *tree)
{
  int size = node_size(tree->root);
  return size;
}

int max(int l, int r)
{
  if (l > r)
    {
      return l;
    }
  else
    {
      return r;
    }
}

int node_depth(node_t *node)
{
  if (!node)
    {
      return 0;
    }
  else
    {
      int l = node_depth(node->left) + 1;
      int r = node_depth(node->right) + 1;
      int maxi = max(l, r);
      return maxi;
    }
}

int tree_depth(tree_t *tree)
{
  if (!tree->root)
    {
      return 0;
    }
  int depth = node_depth(tree->root);
  return depth;
}

node_t *create_node(tree_t *tree, tree_key_t key, elem_t elem)
{
  node_t *tmp = calloc(1, sizeof(node_t));
  tmp->key = key;
  tmp->elem = tree->copy(elem);
  return tmp;
}

bool node_insert(tree_t *tree, node_t *node, tree_key_t key, elem_t elem)
{
  if (tree->comp(node->key, key) == 0 && tree->comp != NULL)
    {
      return false;
    }
  if (tree->comp != NULL)
    {
      if (tree->comp(key, node->key) > 0)
        {
          if (node->right == NULL)
            {
              node->right = create_node(tree, key, elem);
              return true;
            }
          else
            {
              return node_insert(tree, node->right, key, elem);
            }
        }
      else
        {
          if (node->left == NULL)
            {
              node->left = create_node(tree, key, elem);
              return true;
            }
          else
            {
              return node_insert(tree, node->left, key, elem);
            }
        }
    }
  
  return false;
}


bool tree_insert(tree_t *tree, tree_key_t key, elem_t elem)
{
  if (!tree)
    {
      return false;
    }
  else if (tree->root == NULL)
    {
      tree->root = create_node(tree, key, elem);
      return true;
    }
  else
    {
      return node_insert(tree, tree->root, key, elem);
    }
}

bool node_has_key(tree_t *tree, node_t *node, tree_key_t key)
{
  if (node == NULL)
    {
      return false;
    }
  if (tree->comp(node->key, key) == 0)
    {
      return true;
    }
  else if (tree->comp(key, node->key) > 0)
    {
      return node_has_key(tree, node->right, key);
    }
  else
    {
      return node_has_key(tree, node->left, key);
    }
}

bool tree_has_key(tree_t *tree, tree_key_t key)
{
  return node_has_key(tree, tree->root, key);
}

bool node_get(tree_t *tree, node_t *node, tree_key_t key, elem_t *result)
{
  if (!node)
    {
      return false;
    }
  else if (tree->comp(node->key, key) == 0)
    {
      //result = calloc(1, sizeof(elem_t));
      *result = node->elem;
      return true;
    }
  else if (tree->comp(key, node->key) > 0)
    {
      return node_get(tree, node->right, key, result);
    }
  else
    {
      return node_get(tree, node->left, key, result);
    }
}


bool tree_get(tree_t *tree, tree_key_t key, elem_t *result)
{
  bool node = node_get(tree, tree->root, key, result);
  return node;
}


int node_keys(node_t *node, tree_key_t *keys, int i)
{
  if (node->left != NULL)
    {
      i = node_keys(node->left, keys, i);
    }
  keys[i] = node->key;
  i = i + 1;
  if (node->right != NULL)
    {
      i = node_keys(node->right, keys, i);
    }
  return i; 
}

tree_key_t *tree_keys(tree_t *tree)
{
  int size = tree_size(tree);
  tree_key_t *keys = calloc(size, sizeof(tree_key_t));
  node_keys(tree->root, keys, 0);
  return keys;
}


int node_elements(node_t *node, elem_t *elem, int i)
{
  if (node->left != NULL)
    {
      i = node_elements(node->left, elem, i);
    }
  elem[i] = node->elem;
  i = i + 1;
  if (node->right != NULL)
    {
      i = node_elements(node->right, elem, i);
    }
  return i; 
}

elem_t *tree_elements(tree_t *tree)
{
  int size = tree_size(tree);
  elem_t *elem = calloc(size, sizeof(elem_t));
  node_elements(tree->root, elem, 0);
  return elem;
}


node_t *min_node(node_t *node)
{
  node_t *current = node;
  while (current->left != NULL)
    current = current->left;

  return current;
}

node_t *node_remove(tree_t *tree, node_t *node, tree_key_t key)
{
  if (node == NULL) 
    {
      return node;
    }
  if (tree->comp(key, node->key) < 0)
    {
      node->left = node_remove(tree, node->left, key);
    }
  else if (tree->comp(key, node->key) > 0)
    {
      node->right = node_remove(tree, node->right, key);
    }
  else
    {
      if (node->left == NULL)
        {
          node_t *tmp = node->right;
          free(node);
          return tmp;
        }
      else  if  (node->right == NULL)
        {
          node_t *tmp = node->left;
          free(node);
          return tmp;
        }
      node_t *tmp = min_node(node->left);
      node->key = tmp->key;
      node->elem = tmp->elem;
      node->left = node_remove(tree, node->left, tmp->key);       
    }
  return node;
}

bool tree_remove(tree_t *tree, tree_key_t key, elem_t *result)
{
  bool tree_exist = tree_get(tree, key, result);
  if (tree_exist)
    {
      tree->root = node_remove(tree, tree->root, key);
    }
  return tree_exist;
}

bool any_true(bool a, bool b, bool c)
{
  if (a == true || b == true || c == true)
    {
      return true;
    }
  return false;
}

bool tree_apply_inorder(node_t *node, key_elem_apply_fun fun, void *data)
{
  bool a = false;
  bool b = false;
  bool c = false;
  if (node->left)
    {
      a = tree_apply_inorder(node->left, fun, data);
    }
  b = fun(node->key, node->elem, data);
  if (node->right)
    {
      c = tree_apply_inorder(node->right, fun, data);
    }
  return any_true(a, b, c);
}

bool tree_apply_preorder(node_t *node, key_elem_apply_fun fun, void *data)
{
  bool a = false;
  bool b = false;
  bool c = false;
  a = fun(node->key, node->elem, data);
  if (node->left)
    {
      b = tree_apply_preorder(node->left, fun, data);
    }
  if (node->right)
    {
      c = tree_apply_preorder(node->right, fun, data);
    }
  return any_true(a, b, c);
}

bool tree_apply_postorder(node_t *node, key_elem_apply_fun fun, void *data)
{
  bool a = false;
  bool b = false;
  bool c = false;
  if (node->left)
    {
      a = tree_apply_postorder(node->left, fun, data);
    }
  if (node->right)
    {
      b = tree_apply_postorder(node->right, fun, data);
    }
  c = fun(node->key, node->elem, data);
  return any_true(a, b, c);  
}


bool tree_apply(tree_t *tree, enum tree_order order, key_elem_apply_fun fun, void *data)
{
  if (order == 0)
    {
      return tree_apply_inorder(tree->root, fun, data);
    }
  
  else if (order == -1)
    {
      return tree_apply_preorder(tree->root, fun, data); 
    }
  
  else if (order == 1)
    {
      return tree_apply_postorder(tree->root, fun, data);
    }
  return false;
}


void tree_sort_aux(tree_t *tree, node_t *node, tree_key_t *keys, elem_t *elements, int siz)
{
  if (siz == 0)
    {
      if(node)
        {
          node_delete(tree, node, true, true);
        }
    }

  else if (siz == 1)
    {
      node->key = keys[0];
      node->elem = elements[0];
      node_delete(tree, node->left, true, true);
      node_delete(tree, node->right, true, true);
    }
    
  else
    {
      int i = 0;
      int half = siz/2;
      tree_key_t middle_key = keys[half];
      elem_t middle_elem = elements[half];
  
      tree_key_t first_half_key[half];
      elem_t     first_half_elem[half];
      for(; i < half; ++i)
        {
          first_half_key[i] = keys[i];
          first_half_elem[i] = elements[i];
        }

      tree_key_t second_half_key[siz-half-1];
      elem_t     second_half_elem[siz-half-1];

      for(i = i + 1; i < siz; ++i)
        {
          second_half_key[i] = keys[i];
          second_half_elem[i] = elements[i];
        }
      
      node->key = middle_key;
      node->elem = middle_elem;

      if (node->left == NULL)
        {
          node_t *tmp1 = calloc(1, sizeof(node_t));
          node->left = tmp1;
        }
       if (node->right == NULL)
        {
          node_t *tmp2 = calloc(1, sizeof(node_t));
          node->left = tmp2;
        }
       
      tree_sort_aux(tree, node->left, first_half_key, first_half_elem, half);
      tree_sort_aux(tree, node->right, second_half_key, second_half_elem, siz - half - 1);
    }
}

void tree_sort(tree_t *tree)
{
  int round_up = 0;
  float sqrt_tree = sqrt(tree_size(tree) + 1);
  if (round(sqrt_tree) < sqrt_tree)
    {
      round_up = 1;
    }
  if (tree_depth(tree) > round(sqrt_tree) + round_up)
    {
      tree_sort_aux(tree, tree->root, tree_keys(tree), tree_elements(tree), tree_size(tree));
    }
  else
    {
      printf("Databasen är redan sorterad!");
    }
}


