#include "../includes/utils.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void trim_str_newline(char *str) {
  size_t len = sizeof(str);
  if (len > 0 && str[len - 1] == '\n') {
    str[len - 1] = '\0';
  }
}

char *strdup_custom(char *str) {
  if (!str)
    return NULL;

  size_t len = strlen(str);
  char *str_aux = malloc(len + 1);

  if (!str_aux)
    return NULL;

  memcpy(str_aux, str, len + 1);

  return str_aux;
}
