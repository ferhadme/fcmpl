#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "trie.h"

static void traverse_trie(const node *n);
static bool validate_word(const char *word);
static node *put_node(node *parent, const char *word);
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
    printf("[Root]\n");
    int idx = hash(*word);
    printf("put_node() function called with root for char %c\n", *word);
    *(trie->root->children + idx) = put_node(*(trie->root->children + idx), word);
    return true;
}

static node *put_node(node *parent, const char *word)
{
    printf("char %c is called\n", *word);
    if (parent == NULL) {
	parent = create_node(*word);
    } else {
	printf("parent is not null, char is %c\n", parent->ch);
    }
    int idx = hash(*(word + 1));
    if (idx == -1) {
	parent->end_of_word = true;
    } else {
	*(parent->children + idx) = put_node(*(parent->children + idx), word + 1);
    }
    return parent;
}

// TODO Implement function
bool delete(const trie *trie, const char *word);

// TODO Implement function
bool check(const trie *trie, const char *word);

void print_trie(const trie *trie)
{
    node *root = trie->root;
    traverse_trie(root);
}

static void traverse_trie(const node *n)
{
    if (n == NULL) {
	return;
    }
    printf("%c", n->ch);
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	traverse_trie(*(n->children + i));
    }
    if (n->end_of_word) {
	printf("\n");
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
    // assert(0 && "unreachable");
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
