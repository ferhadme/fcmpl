#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#define IS_CPTL_LTR(ch) (ch >= 65 && ch <= 90)
#define IS_LWR_LTR(ch) (ch >= 97 && ch <= 122)
#define IS_VALID_CHAR(ch) (IS_CPTL_LTR(ch) || IS_LWR_LTR(ch))
#define NUMBER_OF_LETTERS 52

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

bool delete(const trie *t, const char *word);

bool check(const trie *t, const char *word);

void complete(const trie *t, const char *word);

void print_trie(const trie *trie);

int hash(char ch);

#endif // TRIE_H
