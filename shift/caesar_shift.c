#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "shift-cipher.h"

char *caesar_shift(char *input, int key, int length) {
  char *output = (char *) malloc(sizeof(char) * 256);
  if(output == NULL) {
    fprintf(stderr, "Failed to allocate memory\n");
    return NULL;
  }
    for (int i = 0; i < length; i++) {
        char arg_char = input[i];
        if ((arg_char >= ASCII_A && arg_char <= ASCII_Z)) {
            output[i] = ((arg_char - ASCII_A + key) % 26) + ASCII_A; // Uppercase letters
        } else if ((arg_char >= ASCII_a && arg_char <= ASCII_z)) {
            output[i] = ((arg_char - ASCII_a + key) % 26) + ASCII_a; // Lowercase letters
        } else {
            output[i] = arg_char; // Copy unchanged character for non-alphabetic characters
        }
    }
    return output;
}