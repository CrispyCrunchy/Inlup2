#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


extern char *strdup(const char *);


typedef union {
  int   i;
  float f;
  char *s;
  char  c;
} answer_t;
typedef bool(*check_func)(char *);
typedef answer_t(*convert_func)(char *);


bool is_edit(char *arg);
bool not_empty(char *str);
bool is_number(char *str);
bool is_yes_no (char *str);
int clear_input_buffer(void);
int read_string(char *buf, int buf_siz);
int ask_question_int(char *question);
char *ask_question_string(char *question);
char ask_question_yes_no(char *question);
char ask_question_edit(char *question);
int ask_question_price(char *question);
char *ask_question_shelf(char *question);
char ask_question_menu(char *question);
answer_t ask_question(char *question, check_func check, convert_func convert);
#endif
