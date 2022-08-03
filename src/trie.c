#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "trie.h"

static void traverse_trie(const node *n, char *prefix, int node_counter);
static bool validate_word(const char *word);
static node *put_node(node *parent, const char *word);
static bool check_node(const node *trie, const char *word);
static bool delete_node(const node *n, const char *word);
static node *create_node(char with);


trie *create_trie()
{
    trie *trie = malloc(sizeof(trie));
    if (trie == NULL) {
	return NULL;
    }

    /*
     * character dot (.) is allocated for root of trie
     */
    node *root = create_node('.');

    trie->root = root;
    trie->size = 0;
    return trie;
}

void free_trie(trie *trie)
{
    // TODO: all nodes in tree also should be freed
    free(trie);
}

bool put(trie *trie, const char *word)
{
    if (!validate_word(word)) {
	return false;
    }
    trie->size++;
    int idx = hash(*word);
    *(trie->root->children + idx) = put_node(*(trie->root->children + idx), word);
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
bool delete(const trie *trie, const char *word);

// TODO Implement
static bool delete_node(const node *n, const char *word);

bool check(const trie *trie, const char *word)
{
    if (!validate_word(word)) {
	return false;
    }
    int idx = hash(*word);
    return check_node(*(trie->root->children + idx), word);
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

void print_trie(const trie *trie)
{
    char *prefix = malloc(sizeof(char) + 1);
    int node_counter = 1;
    traverse_trie(trie->root, prefix, node_counter);
    free(prefix);
}

static void traverse_trie(const node *n, char *prefix, int node_counter)
{
    if (n == NULL) {
	return;
    }
    prefix = realloc(prefix, node_counter);
    prefix[node_counter - 1] = n->ch;
    prefix[node_counter] = '\0';
    if (n->end_of_word) {
	printf("%s\n", prefix);
    }
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	traverse_trie(*(n->children + i), prefix, node_counter + 1);
    }
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
