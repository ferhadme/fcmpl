/*
 * Copyright (c) 2023, Farhad Mehdizada
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "repl.h"
#include "graphviz_cfg.h"

#define BUFFER_SIZE 256

/*
 * Token delimiter in repl command
 */
#define COMMAND_DELIM " "

/*
 * Maximum size of tokens in repl command
 */
#define MAX_TOKEN_SIZE 2

#define COMMAND_STRNCMP_LEN(str) (strlen(str) + 1)

/*
 * Generates target filename with base name and given extension
 */
#define GENERATE_FILE_NAME(target, src, ext)	\
    strcpy(target, src);			\
    strcpy((target) + strlen(src), ext);	\

enum REPL_COMMAND {
    /* Adds new word */
    ADD,
    /* Deletes existing word */
    DELETE,
    /* Checks whether word exists */
    CHECK,
    /* Loads list of valid words from file (separated by newline) */
    LOAD,
    /* Resets trie (removes all nodes except root)*/
    RESET,
    /* Generates file from word tree (reverse process of load) */
    GENERATE,
    /* Visualizes word tree with graph (svg and dot file) */
    VISUALIZE,
    /* Cleans terminal screen */
    CLEAN,
    /* Terminates REPL */
    QUIT,
#ifdef DEBUG
    /* Prints tree in readable format (for debugging) */
    PRINT,
#endif
    /* Assumes that input is not special command and completes given word */
    COMPLETION
};

static bool repl_add(trie *t, char **tokens);
static bool repl_delete(trie *t, char **tokens);
static bool repl_check(trie *t, char **tokens);
static bool repl_load(trie *t, char **tokens);
static bool repl_visualize(trie *t, char **tokens);
static bool repl_generate(trie *t, char **tokens);
static bool repl_complete(trie *t, char **tokens);
static bool repl_reset_trie(trie *t);
static void build_trie(FILE *fp, trie *t);
static enum REPL_COMMAND get_command(const char *token);

bool execute(trie *t, char **tokens)
{
    enum REPL_COMMAND command = get_command(*tokens);

    switch (command) {
    case ADD:
	return repl_add(t, tokens);
    case DELETE:
	return repl_delete(t, tokens);
    case CHECK:
	return repl_check(t, tokens);
    case LOAD:
	return repl_load(t, tokens);
    case RESET:
	return repl_reset_trie(t);
    case VISUALIZE:
	return repl_visualize(t, tokens);
    case GENERATE:
	return repl_generate(t, tokens);
#ifdef DEBUG
    case PRINT:
	print_trie(t);
	return false;
#endif
    case CLEAN:
	system("clear");
	return false;
    case QUIT:
	return true;
    default:
	return repl_complete(t, tokens);
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

static bool repl_add(trie *t, char **tokens)
{
    char *word = *(tokens + 1);
    if (word == NULL) {
	fprintf(stderr, "Word is not provided\n");
	return false;
    }
    if (!put(t, word)) {
	fprintf(stderr, "Invalid word\n");
    }
#ifdef DEBUG
    else {
	visualize_trie_debug(t);
    }
#endif
    return false;
}

static bool repl_delete(trie *t, char **tokens)
{
    char *word = *(tokens + 1);
    if (word == NULL) {
	fprintf(stderr, "Word is not provided\n");
	return false;
    }
    if (!delete(t, word)) {
	fprintf(stderr, "Word doesn't exist\n");
    }
#ifdef DEBUG
    else {
	visualize_trie_debug(t);
    }
#endif
    return false;
}

static bool repl_check(trie *t, char **tokens)
{
    char *word = *(tokens + 1);
    if (word == NULL) {
	fprintf(stderr, "Word is not provided\n");
	return false;
    }
    if (check(t, word)) {
	printf("%s\n", word);
    }
    return false;
}

static bool repl_load(trie *t, char **tokens)
{
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
}

static bool repl_visualize(trie *t, char **tokens)
{
    char *out_name = *(tokens + 1);
    if (out_name == NULL) {
	fprintf(stderr, "Output file name not provided\n");
	return false;
    }

    char dot_out_name[strlen(out_name) + FILE_EXTENSION_LEN];
    GENERATE_FILE_NAME(dot_out_name, out_name, ".dot");

    char svg_out_name[strlen(out_name) + FILE_EXTENSION_LEN];
    GENERATE_FILE_NAME(svg_out_name, out_name, ".svg");

    FILE *dot_fp = fopen(dot_out_name, "w");
    if (dot_fp == NULL) {
	fprintf(stderr, "File couldn't be opened\n");
	return false;
    }

    visualize_trie(dot_fp, dot_out_name, svg_out_name, t);
    fclose(dot_fp);

    return false;
}

static bool repl_generate(trie *t, char **tokens)
{
    char *out_name = *(tokens + 1);
    if (out_name == NULL) {
	fprintf(stderr, "Output file name not provided\n");
	return false;
    }

    char txt_name[strlen(out_name) + FILE_EXTENSION_LEN];
    GENERATE_FILE_NAME(txt_name, out_name, ".txt");

    FILE *txt_fp = fopen(txt_name, "w");
    generate_txt_file(txt_fp, t);
    fclose(txt_fp);

    return false;
}

static bool repl_complete(trie *t, char **tokens)
{
    if (*(tokens + 1) != NULL) {
	fprintf(stderr, "More than one word provided\n");
	return false;
    }
    complete(t, *tokens);
    return false;
}

static bool repl_reset_trie(trie *t)
{
    reset_trie(t);
#ifdef DEBUG
    visualize_trie_debug(t);
#endif
    return false;
}

static void build_trie(FILE *fp, trie *t)
{
    char * line = NULL;
    size_t len = 0;

#ifdef DEBUG
    printf("[DEBUG] Started to load file\n");
#endif

    while (getline(&line, &len, fp) != -1) {
	if (line[0] == '#') continue;
	line[strcspn(line, "\n")] = 0;
	if (!put(t, line)) {
#ifdef DEBUG
	    printf("[DEBUG] Ignoring invalid word %s\n", line);
#endif
	}
#ifdef DEBUG
	else {
	    printf("[DEBUG] Loading word - %s\n", line);
	    visualize_trie_debug(t);
	}
#endif
    }

    if (line) {
        free(line);
    }
}

static enum REPL_COMMAND get_command(const char *token)
{
    if (strncmp(token, ".add", COMMAND_STRNCMP_LEN(".add")) == 0)
	return ADD;
    if (strncmp(token, ".delete", COMMAND_STRNCMP_LEN(".delete")) == 0)
	return DELETE;
    if (strncmp(token, ".check", COMMAND_STRNCMP_LEN(".check")) == 0)
	return CHECK;
    if (strncmp(token, ".load", COMMAND_STRNCMP_LEN(".load")) == 0)
	return LOAD;
    if (strncmp(token, ".visualize", COMMAND_STRNCMP_LEN(".visualize")) == 0)
	return VISUALIZE;
    if (strncmp(token, ".reset", COMMAND_STRNCMP_LEN(".reset")) == 0)
	return RESET;
    if (strncmp(token, ".generate", COMMAND_STRNCMP_LEN(".generate")) == 0)
	return GENERATE;
#ifdef DEBUG
    if (strncmp(token, ".print", COMMAND_STRNCMP_LEN(".print")) == 0)
	return PRINT;
#endif
    if (strncmp(token, ".clean", COMMAND_STRNCMP_LEN(".clean")) == 0)
	return CLEAN;
    if (strncmp(token, ".quit", COMMAND_STRNCMP_LEN(".quit")) == 0)
	return QUIT;
    return COMPLETION;
}
