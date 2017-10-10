#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"
#include "common.h"

typedef struct node node_t;

struct node
{
  tree_key_t key;
  elem_t elem;
  element_copy_fun copy;
  key_free_fun key_free;
  element_free_fun elem_free;
  element_comp_fun comp;
  node_t *left;
  node_t *right;
};

struct tree
{
  node_t *root;
};

tree_t *tree_new(element_copy_fun element_copy, key_free_fun key_free, element_free_fun elem_free, element_comp_fun comp)
{
  tree_t *new = calloc(1, sizeof(tree_t));
  new->root->copy = element_copy;
  new->root->key_free = key_free;
  new->root->elem_free = elem_free;
  new->root->comp = comp;
  return new;
}

void node_delete(node_t *node, bool delete_keys, bool delete_elements)
{
  if (delete_keys == true)
    {
      node_delete(node->left, delete_keys, delete_elements);
      node_delete(node->right, delete_keys, delete_elements);
      node->key_free(node->key);
      node->elem_free(node->elem);
    }
}


void tree_delete(tree_t *tree, bool delete_keys, bool delete_elements)
{
  node_delete(tree->root, delete_keys, delete_elements);
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

node_t *create_node(tree_key_t key, elem_t elem)
{
  node_t *tmp = (node_t *)calloc(1, sizeof(node_t));
  tmp->key = key;
  tmp->elem = elem;
  return tmp;
}

bool node_insert(node_t *node, tree_key_t key, elem_t elem)
{
  if (node->comp(node->key, key) == true && node->comp != NULL)
    {
      return false;
    }
  if (node->comp != NULL)
    {
      if (node->comp(key, node->key) > 0)
        {
          if (node->right == NULL)
            {
              node->right = create_node(key, elem);
              return true;
            }
          else
            {
              return node_insert(node->right, key, elem);
            }
        }
      else
        {
          if (node->left == NULL)
            {
              node->left = create_node(key, elem);
              return true;
            }
          else
            {
              return node_insert(node->left, key, elem);
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
      tree->root = create_node(key, elem);
      return true;
    }
  else
    {
      return node_insert(tree->root, key, elem);
    }
}

bool node_has_key(node_t *node, tree_key_t key)
{
  if (node == NULL)
    {
      return false;
    }
  if (node->comp(node->key, key) == true)
    {
      return true;
    }
  else if (node->comp(key, node->key) > 0)
    {
      return node_has_key(node->right, key);
    }
  else
    {
      return node_has_key(node->left, key);
    }
}

bool tree_has_key(tree_t *tree, tree_key_t key)
{
  return node_has_key(tree->root, key);
}

bool node_get(node_t *node, tree_key_t key, elem_t *result)
{
  if (!node)
    {
      return false;
    }
  else if (node->comp(node->key, key) == true)
    {
      result = calloc(1, sizeof(elem_t));
      result = &node->elem;
      return true;
    }
  else if (node->comp(key, node->key) > 0)
    {
      return node_get(node->right, key, result);
    }
  else
    {
      return node_get(node->left, key, result);
    }
}


bool tree_get(tree_t *tree, tree_key_t key, elem_t *result)
{
  return node_get(tree->root, key, result);
}

  
bool tree_remove(tree_t *tree, tree_key_t key, elem_t *result);
#define tree_update(t, k, e, tmp)                               \
  (tree_remove(t, k, &tmp) ? tree_insert(t, k, e) : false)
tree_key_t *tree_keys(tree_t *tree);
elem_t *tree_elements(tree_t *tree);
bool tree_apply(tree_t *tree, enum tree_order order, key_elem_apply_fun fun, void *data);
