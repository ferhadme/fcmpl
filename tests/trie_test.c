#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "trie.h"

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

/*
 * Stars (*) indicates word terminator (eow)

       .
    /  |  \
   A*  C   D
  /    |    \
 B*    A     B*
 / \   |
C*  Z* B*
|
D*

 */
static void put_test(trie *trie)
{
    assert(put(trie, "ab"));
    assert(put(trie, "abc"));
    assert(put(trie, "db"));
    assert(trie->size == 3);

    assert(put(trie, "cab"));
    assert(put(trie, "abcd"));
    assert(put(trie, "abz"));
    assert(put(trie, "a"));
    assert(trie->size == 7);

    assert(!put(trie, ""));
    assert(!put(trie, "."));
    assert(!put(trie, ".,m"));
    assert(!put(trie, "m./,"));
    assert(trie->size == 7);

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

    printf("All assertions passed for put\n");
}

/*
 * Assuming that check_test(trie) called after put_test(trie), so there are some data in trie to test check function
 */
static void check_test(trie *trie)
{
    assert(check(trie, "a"));
    assert(check(trie, "abc"));
    assert(check(trie, "db"));
    assert(check(trie, "cab"));
    assert(check(trie, "abcd"));
    assert(check(trie, "abz"));
    assert(check(trie, "a"));

    assert(!check(trie, ""));
    assert(!check(trie, "b"));
    assert(!check(trie, "ac"));
    assert(!check(trie, "abzd"));
    assert(!check(trie, "."));
    assert(!check(trie, "cabb"));
    assert(!check(trie, "abcde"));
    assert(!check(trie, "dbd"));

    assert(!check(trie, "."));
    assert(!check(trie, ".,m"));
    assert(!check(trie, "m./,"));

    printf("All assertions passed for check\n");
}

/*
 * Assuming that delete_test(trie) called after put_test(trie), so there are some data in trie to test delete function
 * DELETE_THRESHOLD is 1 for DEBUG mode, so trie should be rebalanced after every single deletion
 */
static void delete_and_rebalancing_test(trie *trie)
{
    node *root = trie->root;
    unsigned int trie_size = trie->size;
    int aidx = hash('a');

/*

       .
    /  |  \
   A*  C   D
  /    |    \
 B*    A     B*
 / \   |
C*  Z* B*
|
D*

 */
    assert(delete(trie, "ab"));
    assert(!check(trie, "ab"));
    trie_size--;

    assert(!delete(trie, "./,"));
    assert(!check(trie, "./,"));
    assert(trie->size == trie_size);

    assert(!delete(trie, ""));
    assert(!check(trie, ""));
    assert(trie->size == trie_size);
/*

       .
    /  |  \
   A*  C   D
  /    |    \
 B     A     B*
/ \    |
C* Z*  B*
|
D*

 */
    assert(!delete(trie, "abcde"));
    assert(!check(trie, "abcde"));
    assert(trie->size == trie_size);

    assert(delete(trie, "abcd"));
    assert(delete(trie, "abc"));
    assert(!check(trie, "abcd"));
    assert(!check(trie, "abc"));
    trie_size -= 2;
    assert(trie->size == trie_size);
/*

       .
    /  |  \
   A*  C   D
  /    |    \
 B     A     B*
  \    |
   Z*  B*

 */
    node *l1_a = *(root->children + aidx);
    assert(l1_a != NULL && l1_a->ch == 'a');

    assert(delete(trie, "abz"));
    assert(delete(trie, "a"));
/*

       .
       |  \
       C   D
       |    \
       A     B*
       |
       B*

 */
    assert(*(root->children + aidx) == NULL);

    assert(delete(trie, "cab"));
    assert(delete(trie, "db"));
/*

       .

 */
    for (int i = 0; i < NUMBER_OF_LETTERS; i++) {
	assert(*(root->children + i) == NULL);
    }

    printf("All assertions passed for delete\n");
}

int main(void) {
#ifndef DEBUG
    static_assert(0 && "DEBUG mode is not enabled");
#endif
    trie *trie = create_trie();
    put_test(trie);
    check_test(trie);
    delete_and_rebalancing_test(trie);
    printf("All tests are passed\n");
    free_trie(trie);
    return 0;
}

