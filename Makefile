CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic

all:
	$(CC) $(CFLAGS) -o fcmpl src/main.c src/trie.c src/repl.c

test:
	$(CC) $(CFLAGS) -o fcmpl_test src/test.c src/trie.c
	./fcmpl_test

clean:
	rm fcmpl fcmpl_test
