/* Program receives 3 parameters: String to cipher, a key which is an integer between
* 0 and 25, and a message length. The output is printf to stdout - a shifted
 * message.
 * Example: ./shift -s "I love mangoes" -k 7 -l 13
 * Output: P svcl thunvz
 * flags:
 *      mandatory:
 *      -k, --key - integer between 0 and 25 as a shift key
 *      -l, --length - string length (security feature)
 *      -s, --string - the string to encrypt, for file see below
 *      optional:
 *      -f, --file - read from a file, reads a string from stdin otherwise
 *      -w, --write - write to a file, creates a file if it doesn't exist. if the flag is not                             specified, the output is written to stdout
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>
#include "shift-cipher.h"

// Defined in caesar_shift.c
char *caesar_shift(char *input, int key, int length);


const char *argp_program_version = "shift-cipher v 1.0";
static char doc[] = "Receives parameters: string to cipher, a key between 0 and 25, and a message length.\nExample: ./shift -s \"I love mangoes\" -k 7 -l 13\nOutput: P svcl thunvz\n";
static char args_doc[] = "string key length";

// Define options
static struct argp_option options[] = {
    {"key", 'k', "NUM", 0, "integer between 0 and 25 as a shift key"},
    {"string", 's', "STRING", 0, "the string to encrypt, up to 1024 bytes long"},
    {"length", 'l', "LEN", 0, "length of the string"},
    {"file", 'f', "FILEPATH", 0, "read from a file"},
    {"write", 'w', "WRITEPATH", 0, "write to a file"},
    {0} // Termination of options array
};

struct arguments {
    char *buffer; // String to encrypt
    int key;      // Shift key
    int length;   // Length of the string
    char *file;   // File to read from
    char *write;  // File to write to
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'k':
            arguments->key = strtol(arg,NULL,10);
            if (arguments->key < 0 || arguments->key > MODULO_MAX) {
                fprintf(stderr, "Error: The key is out of range.\n");
                return ARGP_ERR_UNKNOWN;
            }
            break;
        case 'l':
            arguments->length = strtol(arg,NULL,10);
            if (arguments->length > MAX_BUFFER_LENGTH) {
                fprintf(stderr, "Error: The length is out of range.\n");
                return ARGP_ERR_UNKNOWN;
            }
        if (arguments->length == 0) {
            fprintf(stderr, "Error: The length is not set.\n");
            return ARGP_ERR_UNKNOWN;
        }
            break;
        case 's':
            if (arguments->file != NULL) {
                fprintf(stderr, "Error: Cannot specify both -s and -f.\n");
                return ARGP_ERR_UNKNOWN;
            }
            if (strlen(arg) > MAX_BUFFER_LENGTH) {
                fprintf(stderr, "Error: The string is larger than %d characters.\n", MAX_BUFFER_LENGTH);
                return ARGP_ERR_UNKNOWN;
            }
            arguments->buffer = malloc(strlen(arg));
            if (arguments->buffer == NULL) {
                perror("malloc");
                return ARGP_ERR_UNKNOWN;
            }
            strncpy(arguments->buffer, arg, strlen(arg)); // Copy string into allocated memory
            break;
        case 'f':
            if (arguments->buffer != NULL) {
                fprintf(stderr, "Error: Cannot specify both -s and -f.\n");
                return ARGP_ERR_UNKNOWN;
            }
            arguments->file = arg; // Handle file input here as needed
            break;
        case 'w':
            arguments->write = arg; // Handle file output here as needed
            break;
        case ARGP_KEY_END:
            if (!arguments->buffer && arguments->file == NULL) {
                fprintf(stderr, "Error: Must specify either -s or -f.\n");
                return ARGP_ERR_UNKNOWN;
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN; // Unknown option
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {
    struct arguments arguments = {0}; // Initialize all members to zero

    // Parse command-line arguments
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    if (arguments.length == 0) {
        fprintf(stderr, "Error: The length is not set.\n");
        free(arguments.buffer);
        return ARGP_ERR_UNKNOWN;
    }
    if (arguments.file == NULL && arguments.buffer == NULL) {
        fprintf(stderr, "Error: Must specify either -s or -f.\n");
        return ARGP_ERR_UNKNOWN;
    }
    if (arguments.buffer!=NULL && strlen(arguments.buffer) != arguments.length) {
        fprintf(stderr, "Error: The length of the buffer is not equal to the string.\n");
        return ARGP_ERR_UNKNOWN;
    }
    char *ciphertext = NULL;
    if (arguments.file !=NULL) {
        FILE *f = fopen(arguments.file, "r");
        if (f == NULL) {
            fprintf(stderr, "Error: Failed to open the file %s.\n", arguments.file);
            free(arguments.buffer);
            return ARGP_ERR_UNKNOWN;
        }
        char *buffer = malloc(arguments.length);
        if (fread(buffer, sizeof(char),arguments.length, f) != arguments.length) {
            fprintf(stderr, "Error: Failed to read %d bytes from the file.",arguments.length);
            free(arguments.buffer);
            free(arguments.file);
            return ARGP_ERR_UNKNOWN;
        }
        ciphertext = caesar_shift(buffer, arguments.key, arguments.length);
    } else {
        ciphertext = caesar_shift(arguments.buffer, arguments.key, arguments.length);
    }

    if (ciphertext == NULL) {
        fprintf(stderr, "Error: Failed to cipher.\n");
        free(arguments.buffer);
        return ARGP_ERR_UNKNOWN;
    }

    if (arguments.write != NULL) {
        FILE *f = fopen(arguments.write, "w");
        if (f == NULL) {
            fprintf(stderr, "Error: Failed to open the file %s.\n", arguments.write);
            free(arguments.buffer);
            return ARGP_ERR_UNKNOWN;
        }
        if (fwrite(ciphertext, sizeof(char),arguments.length,f) == arguments.length) {
            return EXIT_SUCCESS;
        }
        fprintf(stderr, "Error: Failed to write to the file %s.\n", arguments.write);
        free(arguments.buffer);
        return ARGP_ERR_UNKNOWN;
    }

    printf("%s\n", ciphertext);
    free(arguments.buffer);
    return EXIT_SUCCESS;
}