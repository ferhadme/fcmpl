#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#define DEBUG 1

#ifdef DEBUG
#define DELETE_THRESHOLD 1
#else
#define DELETE_THRESHOLD 50
#endif

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

void print_trie(const trie *t);

void generate_dot_file(FILE *fp, const trie *t);

int hash(char ch);

#endif // TRIE_H
