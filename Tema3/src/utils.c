#include "../include/utils.h"

#include "../include/parson.h"

// function that returns a value, that i will use in the switch in order to look
// nicer and to recognise the task faster
int clientSwitch(char task[]) {
  int value;
  if (strcmp(task, "register") == 0) {
    value = 1;
  } else if (strcmp(task, "login") == 0) {
    value = 2;
  } else if (strcmp(task, "enter_library") == 0) {
    value = 3;
  } else if (strcmp(task, "get_books") == 0) {
    value = 4;
  } else if (strcmp(task, "get_book") == 0) {
    value = 5;
  } else if (strcmp(task, "add_book") == 0) {
    value = 6;
  } else if (strcmp(task, "delete_book") == 0) {
    value = 7;
  } else if (strcmp(task, "logout") == 0) {
    value = 8;
  } else if (strcmp(task, "exit") == 0) {
    value = 9;
  } else {
    value = 10;
  }
  return value;
}