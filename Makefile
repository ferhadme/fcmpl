CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic

all:
	$(CC) $(CFLAGS) -o fcmpl src/main.c src/trie.c src/test.c
