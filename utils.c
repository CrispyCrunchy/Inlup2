#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"


bool not_empty(char *str)
{
  return strlen(str) > 0;
}


int clear_input_buffer(void)
{
  int c;
  do
    {
      c = getchar();
    }
  while (c != '\n' && c != EOF);
  return 0;
}


bool is_number(char *str)
{
  int len = strlen(str);
  for (int i = 0; i < len; ++i)
  {
    if (i == 0 && str[0] == '-')
    {
      continue;
    }
    else if (!isdigit(str[i]))
    {
      return false;
    }
  } 
  return true;
}

bool is_price(char *str)
{
  int len = strlen(str);
  if (str[0] == '\n' || str[0] == '\0')
    {
      return false;
    }
  else
    {
      for (int i = 0; i < len; ++i)
        {
          if (!isdigit(str[i]) || str[i] == '\n')
            {
              return false;
            }
        }
    }
  return true;
}


int read_string(char *buf, int buf_siz)
{   
  int i = 0;
  char c = getchar();
  while (c != '\n' && c != EOF && i < buf_siz) 
    {
      buf[i] = c;
      i = i +1;
      c = getchar();
    }  
  buf[i] = '\0';

  if (i == buf_siz)
    {
      clear_input_buffer();
    }
  
  return i;
}

bool is_yes_no(char *str)
{
  char s =toupper(str[0]);
  int c = s;
  if (c == 74 || c == 78)
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool is_edit(char *arg)
{
  if (strlen(arg) == 1 && (*arg == 'B' ||
                           *arg == 'b' ||
                           *arg == 'P' ||
                           *arg == 'p' ||
                           *arg == 'L' ||
                           *arg == 'l' ||
                           *arg == 'T' ||
                           *arg == 't' ||
                           *arg == 'A' ||
                           *arg == 'a'))
    {
      return true;
    }
  else 
    {
      return false;
    }
}

bool is_menu(char *arg)
{
  if (strlen(arg) == 1 && (*arg == 'L' ||
                            *arg == 'l' ||
                            *arg == 'T' ||
                            *arg == 't' ||
                            *arg == 'R' ||
                            *arg == 'r' ||
                            *arg == 'G' ||
                            *arg == 'g' ||
                            *arg == 'H' ||
                            *arg == 'h' ||
                            *arg == 'S' ||
                            *arg == 's' ||
                            *arg == 'K' ||
                            *arg == 'k' ||
                            *arg == 'A' ||
                            *arg == 'a'))
    {
      return true;
    }
  else 
    {
      return false;
    }
}

bool is_shelf(char *str)
{
  int len = strlen(str);
  char c = str[0];
  c = toupper(c);
  int q = c;
  if (q >= 65 && q <= 90 && len > 1)
    {
      for (int i = 1; i < len; ++i)
      {
        if (!isdigit(str[i]))
        {
          return false;
        }
      }
      return true;
    }
  return false;
}


char stoc(char *s)
{
  char c = toupper(s[0]);
  return c;
}

char ask_question_edit(char *question)
{
  return ask_question(question, is_edit, (convert_func) stoc).c;
}

char *ask_question_string(char *question)
{
  answer_t answer = ask_question(question, not_empty, (convert_func) strdup);
  return answer.s;
}

int ask_question_int(char *question)
{
  answer_t answer = ask_question(question, is_number, (convert_func) atoi);
  return answer.i;  
}

int ask_question_price(char *question)
{
  answer_t answer = ask_question(question, is_price, (convert_func) atoi);
  return answer.i;
}

char ask_question_yes_no(char *question)
{
  answer_t answer = ask_question(question, is_yes_no, (convert_func) stoc);
  return answer.c;
}

char ask_question_menu(char *question)
{
  return ask_question(question, is_menu, (convert_func) stoc).c;
}

char *ask_question_shelf(char *question)
{
  answer_t answer = ask_question(question, is_shelf, (convert_func) strdup);
  return answer.s;
}


answer_t ask_question(char *question, check_func check, convert_func convert)
{
  int buf_siz = 255;
  char buf[buf_siz];
  bool funCheck = false;
  do
  {
    printf("%s\n", question);
    read_string(buf, buf_siz);
    funCheck = check(buf);
    if (!funCheck)
      {
        printf("Felaktig inmatning. Försök igen.\n");
      }
  }
  while(!funCheck);
  return convert(buf);
}

