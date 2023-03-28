/*
 * Copyright (c) 2023, Farhad Mehdizada
 */

#ifndef REPL_H
#define REPL_H

#include <stdlib.h>
#include <stdbool.h>
#include "trie.h"

#define COMMAND_PROMPT "> "
#define FREE_INPUT(tokens, line)		\
    free(tokens);				\
    free(line);					\

char *get_line();
char **parse_line(char *line);
bool execute(trie *trie, char **tokens);

#endif // REPL_H
