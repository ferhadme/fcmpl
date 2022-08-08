#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

typedef struct node
{
    char ch;
    struct node **children;
    bool end_of_word;
} node;

typedef struct
{
    node *root;
    unsigned int size;
} trie;

trie *create_trie();

void free_trie(trie *t);

bool put(trie *t, const char *word);

bool delete(trie *t, const char *word);

bool check(const trie *t, const char *word);

void complete(const trie *t, const char *word);

void print_trie(const trie *t);

void generate_graph_svg(FILE *fp, const trie *t);

int hash(char ch);

#endif // TRIE_H
