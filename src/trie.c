#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "trie.h"

static void traverse_trie(const node *n, char *prefix, size_t prefix_len);
static bool validate_word(const char *word);
static node *put_node(node *parent, const char *word);
static bool check_node(const node *t, const char *word);
// static bool delete_node(const node *n, const char *word);
static node *get_final_node(node *n, const char *word);
static node *create_node(char with);


trie *create_trie()
{
    trie *t = malloc(sizeof(trie));
    if (t == NULL) {
	return NULL;
    }

    /*
     * character dot (.) is allocated for root of trie
     */
    node *root = create_node('.');

    t->root = root;
    t->size = 0;
    return t;
}

void free_trie(trie *t)
{
    // TODO: all nodes in tree also should be freed
    free(t);
}

bool put(trie *t, const char *word)
{
    if (!validate_word(word)) {
	return false;
    }
    t->size++;
    int idx = hash(*word);
    *(t->root->children + idx) = put_node(*(t->root->children + idx), word);
    return true;
}

static node *put_node(node *parent, const char *word)
{
    if (parent == NULL) {
	parent = create_node(*word);
    }
    int idx = hash(*(word + 1));
    if (idx == -1) {
	parent->end_of_word = true;
    } else {
	*(parent->children + idx) = put_node(*(parent->children + idx), word + 1);
    }
    return parent;
}

// TODO Implement
// bool delete(const trie *t, const char *word);

// TODO Implement
// static bool delete_node(const node *n, const char *word);

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
    strcpy(prefix, word);

    traverse_trie(n, prefix, prefix_len);
    free(prefix);
}

void print_trie(const trie *t)
{
    size_t prefix_len = 1;
    char *prefix = malloc((sizeof(char) * prefix_len) + 1);
    traverse_trie(t->root, prefix, prefix_len);
    printf("---%s---\n", prefix);
}

static void traverse_trie(const node *n, char *prefix, size_t prefix_len)
{
    if (n == NULL) {
	return;
    }
    prefix = realloc(prefix, prefix_len + 1);
    prefix[prefix_len - 1] = n->ch;
    prefix[prefix_len] = '\0';
    if (n->end_of_word) {
	printf("%s\n", prefix);
    }
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	traverse_trie(*(n->children + i), prefix, prefix_len + 1);
    }
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
    n->ch = with;
    n->children = malloc(sizeof(node *) * NUMBER_OF_LETTERS);
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	*(n->children + i) = NULL;
    }
    n->end_of_word = false;
    return n;
}
