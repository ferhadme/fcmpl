#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include "trie.h"
#include "test.h"

static void node_test(const char *word, int n_ch, ...);


/*
    .
   /|\
  A C D
 /  |  \
 B  A   B
/\  |
C Z B
|
D
 */
void put_test()
{
    trie *trie = create_trie();
    put(trie, "ab");
    put(trie, "abc");
    put(trie, "db");
    put(trie, "cab");
    put(trie, "abcd");
    put(trie, "abz");
    put(trie, "a");
    put(trie, "");

    int aidx = hash('a');
    int bidx = hash('b');
    int cidx = hash('c');
    int didx = hash('d');
    int zidx = hash('z');

    node *root = trie->root;
    node *l1_a = *(root->children + aidx);
    assert(l1_a != NULL);
    node *l1_d = *(root->children + didx);
    assert(l1_d != NULL);
    node *l1_c = *(root->children + cidx);
    assert(l1_c != NULL);

    node *l2_b = *(l1_a->children + bidx);
    assert(l2_b != NULL);
    node *l2_b_1 = *(l1_d->children + bidx);
    assert(l2_b_1 != NULL);
    node *l2_a = *(l1_c->children + aidx);
    assert(l2_a != NULL);

    node *l3_c = *(l2_b->children + cidx);
    assert(l3_c != NULL);
    node *l3_z = *(l2_b->children + zidx);
    assert(l3_z != NULL);
    node *l3_b = *(l2_a->children + bidx);
    assert(l3_b != NULL);

    node *l4_d = *(l3_c->children + didx);
    assert(l4_d != NULL);

    node_test("abcd", 4, l1_a->ch, l2_b->ch, l3_c->ch, l4_d->ch);
    node_test("abz", 3, l1_a->ch, l2_b->ch, l3_z->ch);
    node_test("db", 2, l1_d->ch, l2_b_1->ch);
    node_test("cab", 3, l1_c->ch, l2_a->ch, l3_b->ch);

    // TODO: free trie
}

static void node_test(const char *word, int n_ch, ...)
{
    char res[n_ch + 1];
    va_list ptr;
    va_start(ptr, n_ch);
    for (int i = 0; i < n_ch; i++) {
	res[i] = va_arg(ptr, int);
    }
    res[n_ch] = '\0';
    va_end(ptr);

    assert(strcmp(word, res) == 0);
}
