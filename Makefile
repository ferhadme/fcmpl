CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic
SRC = src/main.c src/trie.c src/repl.c
TESTSRC = src/test.c src/trie.c

debug:
	$(CC) -D DEBUG $(CFLAGS) -o fcmpl $(SRC)

release:
	$(CC) $(CFLAGS) -o fcmpl $(SRC)

test:
	$(CC) -D DEBUG $(CFLAGS) -o fcmpl_test $(TESTSRC)
	./fcmpl_test

clean:
	rm fcmpl fcmpl_test
