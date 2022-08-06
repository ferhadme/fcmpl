#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

#define BUFFER_SIZE 256
#define COMMAND_DELIM " "

static char *get_line();
static char **parse_line(char *line);
static void execute();

#define LOAD ".LOAD"
#define PUT ".PUT"
#define DELETE ".DELETE"
#define CHECK ".CHECK"
#define PRINT_ALL ".PRINT_ALL"
#define GENERATE ".GENERATE"
#define QUIT ".QUIT"

int main(void) {
    char *line;
    char **tokens;
    bool quit = false;

    trie *trie = create_trie();

    do {
	printf("> ");
	line = get_line();
	tokens = parse_line(line);
	if (tokens == NULL) {
	    fprintf(stderr, "Bad command\n");
	} else if (strncmp(*tokens, LOAD, 6) == 0) {
	    char *file_name = *(++tokens);
	    if (file_name == NULL) {
		fprintf(stderr, "File name not provided\n");
		continue;
	    }
	    FILE *f = fopen(file_name, "r");
	    if (f == NULL) {
		fprintf(stderr, "File couldn't be opened\n");
		continue;
	    }
	    // TODO: load file and build trie
	} else if (strncmp(*tokens, PUT, 5) == 0) {
	    char *word = *(++tokens);
	    if (word == NULL) {
		fprintf(stderr, "Word is not provided\n");
		continue;
	    }
	    // TODO: put new word into trie
	} else if (strncmp(*tokens, DELETE, 8) == 0) {
	    char *word = *(++tokens);
	    if (word == NULL) {
		fprintf(stderr, "Word is not provided\n");
		continue;
	    }
	    // TODO: delete existing word from trie
	} else if (strncmp(*tokens, CHECK, 7) == 0) {
	    char *word = *(++tokens);
	    if (word == NULL) {
		fprintf(stderr, "Word is not provided\n");
		continue;
	    }
	    // TODO: check if word exists in trie
	} else if (strncmp(*tokens, PRINT_ALL, 11) == 0) {
	    if (*(++tokens) == NULL) {
		fprintf(stderr, "Word is not provided\n");
		continue;
	    }
	    // TODO: print all words in trie
	} else if (strncmp(*tokens, GENERATE, 10) == 0) {
	    char *out_name = *(++tokens);
	    if (out_name == NULL) {
		fprintf(stderr, "Output file name not provided\n");
		continue;
	    }
	    FILE *f = fopen(out_name, "r");
	    if (f == NULL) {
		fprintf(stderr, "File couldn't be opened\n");
		continue;
	    }
	    // TODO: generate graphical image from trie
	} else if (strncmp(*tokens, QUIT, 6) == 0) {
	    quit = true;
	} else {
	    if (*(++tokens) != NULL) {
		fprintf(stderr, "More than one word provided\n");
		continue;
	    }
	    // TODO: complete the word
	}
    } while (!quit);

    free_trie(trie);
    free(tokens);

    return 0;
}

static char *get_line()
{
    size_t buf_size = BUFFER_SIZE;
    char *buffer = malloc(sizeof(char) * buf_size);
    if (buffer == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	exit(1);
    }

    unsigned int position = 0;
    char ch;
    while (true) {
	ch = getchar();
	if (ch == EOF || ch == '\n') {
	    buffer[position] = '\0';
	    break;
	} else {
	    buffer[position] = ch;
	}
	position++;

	if (position >= buf_size) {
	    buf_size += BUFFER_SIZE;
	    buffer = realloc(buffer, buf_size);
	    if (buffer == NULL) {
		fprintf(stderr, "Memory allocation error\n");
		exit(1);
	    }
	}
    }

    return buffer;
}

static char **parse_line(char *line)
{
    char **tokens = malloc(sizeof(char *) * 2);
    if (tokens == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	exit(1);
    }
    char *token;

    token = strtok(line, COMMAND_DELIM);
    if (token == NULL) {
	return NULL;
    }
    tokens[0] = token;

    token = strtok(NULL, COMMAND_DELIM);
    tokens[1] = token;

    if (strtok(NULL, COMMAND_DELIM) != NULL) {
	return NULL;
    }
    return tokens;
}
