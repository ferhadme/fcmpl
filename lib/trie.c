/*
 * Copyright (c) 2023, Farhad Mehdizada
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "trie.h"
#include "graphviz_cfg.h"

#define IS_CPTL_LTR(ch) ((ch) >= 65 && (ch) <= 90)
#define IS_LWR_LTR(ch) ((ch) >= 97 && (ch) <= 122)
#define IS_VALID_CHAR(ch) (IS_CPTL_LTR(ch) || IS_LWR_LTR(ch))

/*
 * Character dot (.) is allocated for root of trie
 */
#define ROOT_CHAR '.'

/*
 * Expands prefix by 1 letter in each step of trie traverse
 */
#define EXPAND_PREFIX(prefix, prefix_len, ch)		\
    prefix = realloc(prefix, (prefix_len) + 1);		\
    if (prefix == NULL) {				\
	fprintf(stderr, "Memory allocation error\n");	\
	return NULL;					\
    }							\
    prefix[(prefix_len) - 1] = ch;			\
    prefix[prefix_len] = '\0';				\

/*
 * Enum for separating different types of nodes in deletion process (eow node, leaf node, orphan node)
 */
enum NODE_TYPE {
    EOW_NODE, LEAF_NODE, ORPHAN_NODE
};

static char *traverse_trie(const node *n, char *prefix, size_t prefix_len, FILE *out);
static bool validate_word(const char *word);
static node *put_node(node *parent, const char *word);
static bool check_node(const node *t, const char *word);
static node *get_final_node(node *n, const char *word);
static node *create_node(char with);
static void free_node(node *n);
static void dot_node(FILE *fp, const node *n);
static void rebuild_trie_if_threshold_passed(trie *t);
static enum NODE_TYPE clean_orphan_nodes(node *n);
static void free_orphan_node(node *n);
static void generate_svg_from_dot(char **args);

trie *create_trie()
{
    trie *t = malloc(sizeof(trie));
    if (t == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return NULL;
    }

    node *root = create_node(ROOT_CHAR);
    if (root == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return NULL;
    }

    t->root = root;
    t->size = 0;
    t->delete_threshold = 0;
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

#ifdef DEBUG
void visualize_trie_debug(const trie *t)
{
    FILE *dot_fp = fopen("out.dot", "w");
    visualize_trie(dot_fp, "out.dot", "out.svg", t);
    fclose(dot_fp);
}
#endif

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
	parent->eow = true;
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
    n->eow = false;
    t->size--;
    t->delete_threshold++;
    rebuild_trie_if_threshold_passed(t);
    return true;
}

/*
 * Rebalances trie if number of deletions reaches DELETE_THRESHOLD
 */
static void rebuild_trie_if_threshold_passed(trie *t)
{
    if (t->delete_threshold < DELETE_THRESHOLD) return;

#ifdef DEBUG
    printf("[DEBUG] Rebuilding the trie...\n");
#endif
    node *root = t->root;
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	node *child = *(root->children + i);

	enum NODE_TYPE type = clean_orphan_nodes(child);
	if (type == ORPHAN_NODE) {
	    *(root->children + i) = NULL;
	}
    }

    t->delete_threshold = 0;
}

#if 0
static void debug_node(const node *n)
{
    printf("Node { ch = %c, eow = %i }\n", n->ch, n->eow);
}
#endif

static enum NODE_TYPE clean_orphan_nodes(node *n)
{
    if (n == NULL) {
	return LEAF_NODE;
    }

    bool reduntant = true;
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	node *child = *(n->children + i);

	enum NODE_TYPE type = clean_orphan_nodes(child);

	if (type == ORPHAN_NODE) {
	    *(n->children + i) = NULL;
	}

	if (type == EOW_NODE) {
	    reduntant = false;
	}
    }

    if (!n->eow && reduntant) {
	free_orphan_node(n);
    } else {
	reduntant = false;
    }

    return reduntant ? ORPHAN_NODE : EOW_NODE;
}

static void free_orphan_node(node *n)
{
    free(n->children);
    free(n);
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
	return n->eow;
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

    prefix = traverse_trie(n, prefix, prefix_len, stdout);
    if (prefix == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return;
    }
    free(prefix);
}

#ifdef DEBUG
void print_trie(const trie *t)
{
    size_t prefix_len = 1;

    char *prefix = malloc((sizeof(char) * prefix_len) + 1);
    if (prefix == NULL) {
    	fprintf(stderr, "Memory allocation error\n");
	return;
    }

    prefix = traverse_trie(t->root, prefix, prefix_len, stdout);
    if (prefix == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return;
    }
    free(prefix);
}
#endif

void generate_txt_file(FILE *fp, const trie *t)
{
    node *root = t->root;
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	node *child = *(root->children + i);

	size_t prefix_len = 1;
	char *prefix = malloc((sizeof(char) * prefix_len) + 1);
	if (prefix == NULL) {
	    fprintf(stderr, "Memory allocation error\n");
	    return;
	}

	prefix = traverse_trie(child, prefix, prefix_len, fp);
	if (prefix == NULL) {
	    fprintf(stderr, "Memory allocation error\n");
	    return;
	}
	free(prefix);
    }
}

static char *traverse_trie(const node *n, char *prefix, size_t prefix_len, FILE *out)
{
    if (n == NULL) {
	return prefix;
    }
    EXPAND_PREFIX(prefix, prefix_len, n->ch);
    if (n->eow) {
	fprintf(out, "%s\n", prefix);
    }
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	prefix = traverse_trie(*(n->children + i), prefix, prefix_len + 1, out);
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

void visualize_trie(FILE *dot_fp, char *dot_out_name, char *svg_out_name, const trie *t)
{
    if (t->size > GRAPH_VISUALIZER_LIMIT) {
	fprintf(stderr, "Size of trie [%d] has reached to graph visualizer limit [%d]\n",
		t->size, GRAPH_VISUALIZER_LIMIT);
	return;
    }
    generate_dot_file(dot_fp, t);

    char *args[] = {
	DOT_LAYOUT_ENGINE, DOT_GRAPH_FORMAT_FLAG, dot_out_name, "-o", svg_out_name, NULL
    };

    generate_svg_from_dot(args);
}

void generate_dot_file(FILE *fp, const trie *t)
{
    fprintf(fp, "digraph {\n");

    node *root = t->root;
    fprintf(fp, DOT_FILE_ROOT_NODE_FORMAT,
	    (void *) root, root->ch, ROOT_NODE_COLOR);
    dot_node(fp, root);

    fprintf(fp, "}\n");
}

static void dot_node(FILE *fp, const node *n)
{
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	node *child = *(n->children + i);
	if (child != NULL) {
	    fprintf(fp, DOT_FILE_CHILD_NODE_FORMAT,
		    (void *) child, child->ch,
		    child->eow ? EOW_CHILD_NODE_COLOR : CHILD_NODE_COLOR);
	    fprintf(fp, "  \"%p\" -> \"%p\"\n", (void *) n, (void *) child);
	    dot_node(fp, child);
	}
    }
}

static void generate_svg_from_dot(char **args)
{
    if (fork() == 0) {
	execvp("dot", args);
	fprintf(stderr, "Unknown args\n");
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
    if (*word == '\0') {
	return false;
    }
    while (*word != '\0') {
	if (!IS_VALID_CHAR(*word)) {
	    return false;
	}
	word++;
    }
    return true;
}

static node *create_node(char with)
{
    node *n = malloc(sizeof(node));
    if (n == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return NULL;
    }
    n->ch = with;
    n->children = calloc(NUMBER_OF_LETTERS, sizeof(node *));
    if (n->children == NULL) {
	fprintf(stderr, "Memory allocation error\n");
	return NULL;
    }
    n->eow = false;
    return n;
}
