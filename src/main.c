#include <stdio.h>
#include "repl.h"
#include "trie.h"

int main(void) {
    trie *t = create_trie();
    bool status = false;

    do {
	printf("%s", COMMAND_PROMPT);
	char *line = get_line();
	char **tokens = parse_line(line);

	if (tokens == NULL) {
	    fprintf(stderr, "Bad command\n");
	    continue;
	}

	status = execute(t, tokens);

	FREE_INPUT(tokens, line);
    } while (!status);

    free_trie(t);

    return 0;
}
