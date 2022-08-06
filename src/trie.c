#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "trie.h"

#define IS_CPTL_LTR(ch) (ch >= 65 && ch <= 90)
#define IS_LWR_LTR(ch) (ch >= 97 && ch <= 122)
#define IS_VALID_CHAR(ch) (IS_CPTL_LTR(ch) || IS_LWR_LTR(ch))
#define NUMBER_OF_LETTERS 52

#define EXPAND_PREFIX(prefix, prefix_len, ch)		\
    prefix = realloc(prefix, prefix_len + 1);		\
    if (prefix == NULL) {				\
	fprintf(stderr, "Memory allocation error\n");	\
	return NULL;					\
    }							\
    prefix[prefix_len - 1] = ch;			\
    prefix[prefix_len] = '\0';				\

static char *traverse_trie(const node *n, char *prefix, size_t prefix_len);
static bool validate_word(const char *word);
static node *put_node(node *parent, const char *word);
static bool check_node(const node *t, const char *word);
// static bool delete_node(const node *n, const char *word);
static node *get_final_node(node *n, const char *word);
static node *create_node(char with);
static void free_node(node *n);


trie *create_trie()
{
    trie *t = malloc(sizeof(trie));
    if (t == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return NULL;
    }

    /*
     * character dot (.) is allocated for root of trie
     */
    node *root = create_node('.');
    if (root == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return NULL;
    }

    t->root = root;
    t->size = 0;
    return t;
}

void free_trie(trie *t)
{
    free_node(t->root);
    free(t);
}

static void free_node(node *n)
{
    if (n == NULL) {
	return;
    }

    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	free_node(*(n->children + i));
    }
    free(n->children);
    free(n);
}

bool put(trie *t, const char *word)
{
    if (!validate_word(word)) {
	return false;
    }
    int idx = hash(*word);
    *(t->root->children + idx) = put_node(*(t->root->children + idx), word);
    t->size++;
    return true;
}

static node *put_node(node *parent, const char *word)
{
    if (parent == NULL) {
	parent = create_node(*word);
	if (parent == NULL) {
	    fprintf(stderr, "Memory allocation error\n");
	    return NULL;
	}
    }
    int idx = hash(*(word + 1));
    if (idx == -1) {
	parent->end_of_word = true;
    } else {
	*(parent->children + idx) = put_node(*(parent->children + idx), word + 1);
    }
    return parent;
}

bool delete(trie *t, const char *word)
{
    if (!validate_word(word)) {
	return false;
    }

    int idx = hash(*word);
    node *n = get_final_node(*(t->root->children + idx), word);
    if (n == NULL) {
	return false;
    }
    n->end_of_word = false;
    t->size--;
    return true;
}

bool check(const trie *t, const char *word)
{
    if (!validate_word(word)) {
	return false;
    }
    int idx = hash(*word);
    return check_node(*(t->root->children + idx), word);
}

static bool check_node(const node *n, const char *word)
{
    if (n == NULL) {
	return false;
    }
    int idx = hash(*(word + 1));
    if (idx == -1 && n != NULL) {
	return n->end_of_word;
    }
    return check_node(*(n->children + idx), word + 1);
}

void complete(const trie *t, const char *word)
{
    if (!validate_word(word)) {
	return;
    }
    int idx = hash(*word);
    node *n = get_final_node(*(t->root->children + idx), word);

    size_t prefix_len = strlen(word);
    char *prefix = malloc((sizeof(char) * prefix_len) + 1);
    if (prefix == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return;
    }
    strcpy(prefix, word);

    prefix = traverse_trie(n, prefix, prefix_len);
    if (prefix == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return;
    }
    free(prefix);
}

void print_trie(const trie *t)
{
    size_t prefix_len = 1;
    char *prefix = malloc((sizeof(char) * prefix_len) + 1);
    if (prefix == NULL) {
    	fprintf(stderr, "Memory allocation error\n");
	return;
    }
    prefix = traverse_trie(t->root, prefix, prefix_len);
    if (prefix == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return;
    }
    free(prefix);
}

static char *traverse_trie(const node *n, char *prefix, size_t prefix_len)
{
    if (n == NULL) {
	return prefix;
    }
    EXPAND_PREFIX(prefix, prefix_len, n->ch);
    if (n->end_of_word) {
	printf("%s\n", prefix);
    }
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	prefix = traverse_trie(*(n->children + i), prefix, prefix_len + 1);
    }
    return prefix;
}

static node *get_final_node(node *n, const char *word)
{
    if (n == NULL) {
	return NULL;
    }
    int idx = hash(*(word + 1));
    if (idx == -1 && n != NULL) {
	return n;
    }
    return get_final_node(*(n->children + idx), word + 1);
}

int hash(char ch)
{
    if (IS_CPTL_LTR(ch)) {
	return ch - 65;
    }
    if (IS_LWR_LTR(ch)) {
	return ch - 71;
    }
    return -1;
}

static bool validate_word(const char *word)
{
    const char *tmp = word;
    while (*word != '\0') {
	if (!IS_VALID_CHAR(*word)) {
	    return false;
	}
	word++;
    }
    return *tmp != *word;
}

static node *create_node(char with)
{
    node *n = malloc(sizeof(node));
    if (n == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return NULL;
    }
    n->ch = with;
    n->children = malloc(sizeof(node *) * NUMBER_OF_LETTERS);
    if (n->children == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return NULL;
    }
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	*(n->children + i) = NULL;
    }
    n->end_of_word = false;
    return n;
}
