#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "repl.h"

#define BUFFER_SIZE 256
#define COMMAND_DELIM " "
#define MAX_TOKEN_SIZE 2

#define LOAD ".load"
#define PUT ".put"
#define DELETE ".delete"
#define CHECK ".check"
#define PRINT ".print"
#define GENERATE ".generate"
#define QUIT ".quit"

#define COMMAND_STRNCMP_LEN(str) (strlen(str) + 1)

bool execute(trie *t, char **tokens)
{
    (void) t;
    if (strncmp(*tokens, QUIT, COMMAND_STRNCMP_LEN(QUIT)) == 0) {
	return true;
    }

    if (strncmp(*tokens, LOAD, COMMAND_STRNCMP_LEN(LOAD)) == 0) {
	char *file_name = *(tokens + 1);
	if (file_name == NULL) {
	    fprintf(stderr, "File name not provided\n");
	    return false;
	}
	FILE *f = fopen(file_name, "r");
	if (f == NULL) {
	    fprintf(stderr, "File couldn't be opened\n");
	    return false;
	}
	// TODO: load file and build trie
	fclose(f);
    }

    else if (strncmp(*tokens, PUT, COMMAND_STRNCMP_LEN(PUT)) == 0) {
	char *word = *(tokens + 1);
	if (word == NULL) {
	    fprintf(stderr, "Word is not provided\n");
	    return false;
	}
	// TODO: put new word into trie
    }

    else if (strncmp(*tokens, DELETE, COMMAND_STRNCMP_LEN(DELETE)) == 0) {
	char *word = *(tokens + 1);
	if (word == NULL) {
	    fprintf(stderr, "Word is not provided\n");
	    return false;
	}
	// TODO: delete existing word from trie
    }

    else if (strncmp(*tokens, CHECK, COMMAND_STRNCMP_LEN(CHECK)) == 0) {
	char *word = *(tokens + 1);
	if (word == NULL) {
	    fprintf(stderr, "Word is not provided\n");
	    return false;
	}
	// TODO: check if word exists in trie
    }

    else if (strncmp(*tokens, PRINT, COMMAND_STRNCMP_LEN(PRINT)) == 0) {
	// TODO: print all words in trie
    }

    else if (strncmp(*tokens, GENERATE, COMMAND_STRNCMP_LEN(GENERATE)) == 0) {
	char *out_name = *(tokens + 1);
	if (out_name == NULL) {
	    fprintf(stderr, "Output file name not provided\n");
	    return false;
	}
	FILE *f = fopen(out_name, "r");
	if (f == NULL) {
	    fprintf(stderr, "File couldn't be opened\n");
	    return false;
	}
	// TODO: generate graphical image from trie
    }

    else {
	if (*(tokens + 1) != NULL) {
	    fprintf(stderr, "More than one word provided\n");
	    return false;
	}
	// TODO: complete the word
    }

    return false;
}

char *get_line()
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
	if (ch == EOF) {
	    printf("\nHave a good day!\n");
	    exit(0);
	}
	if (ch == '\n') {
	    buffer[position] = '\0';
	    return buffer;
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

char **parse_line(char *line)
{
    char **tokens = malloc(sizeof(char *) * MAX_TOKEN_SIZE);
    if (tokens == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	exit(1);
    }
    char *token;

    token = strtok(line, COMMAND_DELIM);
    if (token == NULL) {
	FREE_INPUT(tokens, line);
	return NULL;
    }
    tokens[0] = token;

    token = strtok(NULL, COMMAND_DELIM);
    tokens[1] = token;

    if (strtok(NULL, COMMAND_DELIM) != NULL) {
	FREE_INPUT(tokens, line);
	return NULL;
    }

    return tokens;
}
