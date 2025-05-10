#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
  char *string;
  size_t length;
  size_t capacity;
} Str;

Str str_from(char *str) {
  char *dyn_str = malloc(strlen(str)+1);
  if (!dyn_str) {
    perror("malloc failure");
    exit(1);
  }
  strcpy(dyn_str, str);
  Str result;
  result.string = dyn_str;
  result.length = strlen(str);
  result.capacity = result.length+1;
  return result;
}

void str_free(Str *str) {
  free(str->string);
  str->capacity = 0;
  str->length = 0;
}

void str_print(Str str) {
  printf("String: %s\n", str.string);
  printf("Length: %zu\n", str.length);
  printf("Capacity: %zu\n", str.capacity);
}

Str str_append(Str *s1, Str *s2) {
  s1->length = s1->length+s2->length;
  s1->capacity = s1->length+1;
  char *new_str = realloc(s1->string, s1->capacity);
  if (!new_str) {
    perror("realloc failed");
    exit(1);
  }
  s1->string = new_str;
  memcpy(s1->string + s1->length - s2->length, s2->string, s2->length + 1);
  return *s1;
}

int main() {
  Str name = str_from("Phillip");
  Str hobby = str_from(" Programs");
  name = str_append(&name, &hobby);
  str_print(name);
  str_free(&name);
  str_free(&hobby);
  char *new_name = "Phillip";
  return 0;
}