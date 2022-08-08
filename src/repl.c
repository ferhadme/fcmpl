#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
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

#define DOT_LAYOUT_ENGINE "dot"
#define DOT_GRAPH_FORMAT_FLAG "-Tsvg"
#define FILE_EXTENSION_LEN 5

#define COMMAND_STRNCMP_LEN(str) (strlen(str) + 1)
#define GENERATE_FILE_NAME(target, src, ext)	\
    strcpy(target, src);			\
    strcpy(target + strlen(src), ext);		\


static void build_trie(FILE *fp, trie *t);
static void generate_svg_from_dot(char **args);

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
	FILE *fp = fopen(file_name, "r");
	if (fp == NULL) {
	    fprintf(stderr, "File couldn't be opened\n");
	    return false;
	}
	build_trie(fp, t);
	fclose(fp);
    }

    else if (strncmp(*tokens, PUT, COMMAND_STRNCMP_LEN(PUT)) == 0) {
	char *word = *(tokens + 1);
	if (word == NULL) {
	    fprintf(stderr, "Word is not provided\n");
	    return false;
	}
	put(t, word);
    }

    else if (strncmp(*tokens, DELETE, COMMAND_STRNCMP_LEN(DELETE)) == 0) {
	char *word = *(tokens + 1);
	if (word == NULL) {
	    fprintf(stderr, "Word is not provided\n");
	    return false;
	}
	delete(t, word);
    }

    else if (strncmp(*tokens, CHECK, COMMAND_STRNCMP_LEN(CHECK)) == 0) {
	char *word = *(tokens + 1);
	if (word == NULL) {
	    fprintf(stderr, "Word is not provided\n");
	    return false;
	}
	if (check(t, word)) {
	    printf("%s\n", word);
	}
    }

    else if (strncmp(*tokens, PRINT, COMMAND_STRNCMP_LEN(PRINT)) == 0) {
	print_trie(t);
    }

    else if (strncmp(*tokens, GENERATE, COMMAND_STRNCMP_LEN(GENERATE)) == 0) {
	char *out_name = *(tokens + 1);
	if (out_name == NULL) {
	    fprintf(stderr, "Output file name not provided\n");
	    return false;
	}

	char dot_name[strlen(out_name) + FILE_EXTENSION_LEN];
	GENERATE_FILE_NAME(dot_name, out_name, ".dot");

	FILE *dot_fp = fopen(dot_name, "w");
	if (dot_fp == NULL) {
	    fprintf(stderr, "File couldn't be opened\n");
	    return false;
	}
	generate_dot_file(dot_fp, t);
	fclose(dot_fp);

	char graph_out[strlen(out_name) + 5];
	GENERATE_FILE_NAME(graph_out, out_name, ".svg");

	char *args[] = {
	    DOT_LAYOUT_ENGINE, DOT_GRAPH_FORMAT_FLAG, dot_name, "-o", graph_out, NULL
	};

	generate_svg_from_dot(args);
    }

    else {
	if (*(tokens + 1) != NULL) {
	    fprintf(stderr, "More than one word provided\n");
	    return false;
	}
	complete(t, *tokens);
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

static void generate_svg_from_dot(char **args)
{
    if (fork() == 0) {
	execvp("dot", args);
	printf("Unknown args\n");
    }
}

static void build_trie(FILE *fp, trie *t)
{
    char * line = NULL;
    size_t len = 0;

    printf("[INFO] Started to load file\n");

    while (getline(&line, &len, fp) != -1) {
	line[strcspn(line, "\n")] = 0;
	if (!put(t, line)) {
	    printf("[INFO] Ignoring invalid word %s\n", line);
	}
    }

    if (line) {
        free(line);
    }
}
