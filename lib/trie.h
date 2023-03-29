/*
 * Copyright (c) 2023, Farhad Mehdizada
 */

#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

/*
 * Defines how many deletions needed to rebuild the trie
 */
#ifdef DEBUG
#define DELETE_THRESHOLD 1
#else
#define DELETE_THRESHOLD 50
#endif

/*
 * [A-Za-z]
 */
#define NUMBER_OF_LETTERS 52

/*
 * Maximum trie size allowed to build graph visualizer
 */
#define GRAPH_VISUALIZER_LIMIT 30

typedef struct node
{
    char ch;
    struct node **children;
    bool eow; // end of word
} node;

typedef struct
{
    node *root;
    unsigned int size;
    unsigned int delete_threshold;
} trie;

trie *create_trie();

void free_trie(trie *t);

bool put(trie *t, const char *word);

bool delete(trie *t, const char *word);

bool check(const trie *t, const char *word);

void complete(const trie *t, const char *word);

#ifdef DEBUG
void print_trie(const trie *t);
#endif

void generate_txt_file(FILE *fp, const trie *t);

void generate_dot_file(FILE *fp, const trie *t);

void visualize_trie(FILE *dot_fp, char *dot_out_name, char *svg_out_name, const trie *t);

#ifdef DEBUG
void visualize_trie_debug(const trie *t);
#endif

int hash(char ch);

#endif // TRIE_H
