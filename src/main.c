#include <stdio.h>
#include "repl.h"
#include "trie.h"

int main(void) {
    trie *t = create_trie();
    if (t == NULL) {
	fprintf(stderr, "REPL couldn't be initialized\n");
	return 1;
    }

    bool termination = false;

    do {
	printf("%s", COMMAND_PROMPT);
	char *line = get_line();
	char **tokens = parse_line(line);

	if (tokens == NULL) {
	    fprintf(stderr, "Bad command\n");
	    continue;
	}

	termination = execute(t, tokens);

	FREE_INPUT(tokens, line);
    } while (!termination);

    free_trie(t);

    printf("Have a good day!\n");
    return 0;
}
