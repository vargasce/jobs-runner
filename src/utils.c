#include "../includes/utils.h"
#include <stddef.h>

void trim_str_newline(char *str) {
  size_t len = sizeof(str);
  if (len > 0 && str[len - 1] == '\n') {
    str[len - 1] = '\0';
  }
}
