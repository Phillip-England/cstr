#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// A structure to represent a dynamic string with length and capacity metadata
typedef struct {
  char *string;
  size_t byte_count;
  size_t capacity;
} Str;

// Creates a dynamic string from a C string literal or char array
// Allocates memory, copies the contents, and initializes metadata
Str string_from(const char *str) {
	size_t len = strlen(str);
  char *dyn_str = malloc(len + 1);
  if (!dyn_str) {
    perror("malloc failure");
    exit(1);
  }
  memcpy(dyn_str, str, len + 1); // accounts for null terminator
  Str result;
  result.string = dyn_str;
  result.byte_count = len;
  result.capacity = result.byte_count + 1;
  return result;
}

// Frees memory allocated for a dynamic string and resets its metadata
void string_free(Str *str) {
  free(str->string);
	str->string = NULL;
  str->capacity = 0;
  str->byte_count = 0;
}

// Prints the contents, length, and capacity of a Str
void string_print(Str str) {
  if (str.string) {
    printf("String: %s\n", str.string);
  } else {
    printf("String: (null)\n");
  }
  printf("Length: %zu\n", str.byte_count);
  printf("Capacity: %zu\n", str.capacity);
}

// Appends the contents of one Str (s2) to another (s1), reallocating if needed
void string_append(Str *s1, const Str *s2) {
  size_t new_byte_count = s1->byte_count + s2->byte_count;
  if (new_byte_count + 1 > s1->capacity) {
    size_t new_capacity = s1->capacity * 2;
    if (new_capacity < new_byte_count + 1) {
      new_capacity = new_byte_count + 1;
    }
    char *new_str = realloc(s1->string, new_capacity);
    if (!new_str) {
      perror("realloc failed");
      exit(1);
    }
    s1->string = new_str;
    s1->capacity = new_capacity;
  }
  memcpy(s1->string + s1->byte_count, s2->string, s2->byte_count + 1); // include null
  s1->byte_count = new_byte_count;
}

// Appends a single character to a Str by wrapping it as a temporary Str
void string_append_char(Str *str, char ch) {
	char ch2[2] = { ch, '\0' };
	Str temp = string_from(ch2);
	string_append(str, &temp);
	string_free(&temp);
}

int utf8_char_length(uint8_t byte) {
	if ((byte & 0x80) == 0x00) return 1;
	else if ((byte & 0xE0) == 0xC0) return 2;
	else if ((byte & 0xf0) == 0xE0) return 3;
	else if ((byte & 0xf8) == 0xF0) return 4;
	else return -1;
}

size_t string_char_count(const Str *s) {
	const unsigned char *p = (const unsigned char *)s->string;
	size_t count = 0;
	while (*p) {
		int len = utf8_char_length(*p);
		if (len < 1) {
			p++; // skip invalid
		} else {
			p += len;
		}
		count++;
	}
	return count;
}

// gets the pointer from a Str
unsigned char *string_pointer(const Str *s) {
	unsigned char *p =  (unsigned char *)s->string;
	return p;
}

//==================
// TESTS
//==================

void string_from_test(void) {
	Str str = string_from("some string");
	if (strcmp(str.string, "some string") != 0) {
		perror("FAILED STRING CREATION");
		exit(0);
	}
	if (str.byte_count != 11) {
		perror("FAILED STRING BYTE COUNT");
		exit(0);
	}
	const char *source = "a static string";
	Str derived = string_from(source);
	if (strcmp(derived.string, source) != 0) {
		perror("FAILED STRING CREATION EXTERNAL SOURCE");
		exit(0);
	}
	string_free(&str);
	string_free(&derived);
}

void string_append_test(void) {
	Str first = string_from("Hello, ");
	Str second = string_from("World!");
	string_append(&first, &second);
	if (strcmp(first.string, "Hello, World!") != 0) {
		perror("FAILED STRING APPEND");
		exit(0);
	}
	string_free(&first);
	string_free(&second);
}

void string_loop_test(void) {
	Str some_str = string_from("Hello, World!");
	unsigned char *p = string_pointer(&some_str);
	while (*p) {
		printf("%c\n", *p);
		p++;
	}
}

void string_utf8_test(void) {
	Str two_byte_str = string_from("éñøç");
	if (string_char_count(&two_byte_str) != 4) {
		perror("EXPECTED 4 CHARACTERS");
		exit(0);
	}
	Str three_byte_str = string_from("✓→€中ह");
	if (string_char_count(&three_byte_str) != 5) {
		perror("EXPECTED 5 CHARACTERS");
		exit(0);
	}
}

void print_bytes(unsigned char *p, int length) {
	for (int j = 0; j < length; j++) {
		unsigned char byte = p[j];
		for (int i = 7; i >= 0; i--) {
			int bit = (byte >> i) & 1;
			printf("%d", bit);
		}
		printf("\n");
	}
}


void string_test(void) {
	// string_from_test();
	// string_append_test();
	// string_loop_test();
	// string_utf8_test();


	Str str = string_from("some Â string");
	unsigned char *p = string_pointer(&str);

	while (*p) {
		int len = utf8_char_length(*p);
		switch (len) {
			case 1: {
				printf("[%c]\n", *p);
				print_bytes(p, len);
				// how to print the binary value of the byte (or bytes?)
				break;
			}
			case 2: {
				char buffer[3];
				memcpy(buffer, p, 2);
				buffer[2] = '\0';
				printf("[%s]\n", buffer);
				print_bytes(p, len);
				break;
			}
			case 3: {
				char buffer[4];
				memcpy(buffer, p, 3);
				buffer[3] = '\0';
				printf("[%s]\n", buffer);
				print_bytes(p, len);
				break;
			}
			case 4: {
				char buffer[5];
				memcpy(buffer, p, 4);
				buffer[4] = '\0';
				printf("[%s]\n", buffer);
				print_bytes(p, len);
				break;
			}
			default: {
				printf("major error");
				break;
			}
		}
		p = p + len;
	}
	
	// char *malicous = "i contain => ​  <= a zero width Â";
	// const unsigned char *p = (const unsigned char *)malicous;
	// while (*p) {
	// 	// size_t remaining = strlen((const char *)p);
	// 	unsigned char byte = *p;
	// 	if (*p >= 0x20 && *p <= 0x7E) {
	// 		printf("Allowed: %c\n", byte);
	// 	} else {
	// 		printf("Not Allowed: %c\n", byte);
	// 	}
	// 	p++;
	// }
	

}

int main(void) {
	string_test();
	return 0;
}


