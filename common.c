#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "common.h"

elem_t *element_copy_fun(elem_t);

void *element_free_fun(elem_t);

int *element_comp_fun(elem_t, elem_t);

bool *elem_apply_fun(elem_t elem, void *data);
