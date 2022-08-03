#include <stdio.h>
#include <stdlib.h>
#include "trie.h"
#include "test.h"

void build_trie_from_file(FILE *cf)
{
    (void) cf;
}

int main(int argc, char **argv) {
    (void) argc;
    char *prog_name = *argv++;
    char *config_file = *argv++;

    if (config_file == NULL) {
        fprintf(stderr, "ERROR: config file is required\n"
		"  usage: %s config_file\n", prog_name);
        exit(1);
    }

    // TODO: open config_file and load all words to your trie
    // FILE *cf = fopen(config_file, "r");
    // if (cf == NULL) {
    // 	fprintf(stderr, "ERROR: error happened while opening config file\n");
    //     exit(1);
    // }

    trie *trie = create_trie();
    put_test(trie);
    print_trie(trie);
    check_test(trie);

    // TODO: write interactive shell (REPL) that reads words
    // TODO: prints all possible completions for expected word

    return 0;
}
