#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "repl.h"
#include "graphviz_cfg.h"

#define BUFFER_SIZE 256
#define COMMAND_DELIM " "
#define MAX_TOKEN_SIZE 2

#define COMMAND_STRNCMP_LEN(str) (strlen(str) + 1)
#define GENERATE_FILE_NAME(target, src, ext)	\
    strcpy(target, src);			\
    strcpy((target) + strlen(src), ext);	\

enum REPL_COMMAND {
    /* adds new word */
    ADD,
    /* deletes existing word */
    DELETE,
    /* checks whether word exists */
    CHECK,
    /* loads list of valid words from file (separated by newline) */
    LOAD,
    /* generates file from word tree (reverse process of load) */
    GENERATE,
    /* visualizes word tree with graph (svg and dot file) */
    VISUALIZE,
    /* terminates REPL */
    QUIT,
#ifdef DEBUG
    /* prints tree in readable format (for debugging) */
    PRINT,
#endif
    /* assumes that input is not special command and completes given word */
    COMPLETION
};

static bool repl_add(trie *t, const char **tokens);
static bool repl_delete();
static void build_trie(FILE *fp, trie *t);
static void generate_svg_from_dot(char **args);
static enum REPL_COMMAND get_command(const char *token);

bool execute(trie *t, char **tokens)
{
    enum REPL_COMMAND command = get_command(*tokens);

    switch (command) {
    case ADD:
	
    case DELETE:
	char *word = *(tokens + 1);
	if (word == NULL) {
	    fprintf(stderr, "Word is not provided\n");
	    return false;
	}
	delete(t, word);
	return false;

    case CHECK:
	char *word = *(tokens + 1);
	if (word == NULL) {
	    fprintf(stderr, "Word is not provided\n");
	    return false;
	}
	if (check(t, word)) {
	    printf("%s\n", word);
	}
	return false;

    case LOAD:
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
	return false;

    case VISUALIZE:
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

	char graph_out[strlen(out_name) + FILE_EXTENSION_LEN];
	GENERATE_FILE_NAME(graph_out, out_name, ".svg");

	char *args[] = {
	    DOT_LAYOUT_ENGINE, DOT_GRAPH_FORMAT_FLAG, dot_name, "-o", graph_out, NULL
	};

	generate_svg_from_dot(args);
	return false;

    case GENERATE:
	break;

#ifdef DEBUG
    case PRINT:
	print_trie(t);
	return false;
#endif

    case QUIT:
	return true;

    default:
	if (*(tokens + 1) != NULL) {
	    fprintf(stderr, "More than one word provided\n");
	    return false;
	}
	complete(t, *tokens);
	return false;
    }
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

static bool repl_add(const char **tokens, trie *t)
{
    char *word = *(tokens + 1);
    if (word == NULL) {
	fprintf(stderr, "Word is not provided\n");
	return false;
    }
    put(t, word);
    return false;
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

static enum REPL_COMMAND get_command(const char *token)
{
    if (strncmp(*token, ".add", COMMAND_STRNCMP_LEN(".add")) == 0)
	return ADD;
    if (strncmp(*token, ".delete", COMMAND_STRNCMP_LEN(".delete")) == 0)
	return DELETE;
    if (strncmp(*token, ".check", COMMAND_STRNCMP_LEN(".check")) == 0)
	return CHECK;
    if (strncmp(*token, ".load", COMMAND_STRNCMP_LEN(".load")) == 0)
	return LOAD;
    if (strncmp(*token, ".visualize", COMMAND_STRNCMP_LEN(".visualize")) == 0)
	return VISUALIZE;
    if (strncmp(*token, ".generate", COMMAND_STRNCMP_LEN(".generate")) == 0)
	return GENERATE;
#ifdef DEBUG
    if (strncmp(*token, ".print", COMMAND_STRNCMP_LEN(".print")) == 0)
	return PRINT;
#endif
    if (strncmp(*token, ".quit", COMMAND_STRNCMP_LEN(".quit")) == 0)
	return QUIT;
    return COMPLETION;
}
