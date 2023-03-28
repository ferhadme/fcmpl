CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11
DEBUGFLAGS = -D DEBUG
SRCDIR = src
LIBDIR = lib
BINDIR = bin
BUILDDIR = build
TESTDIR = tests
TARGET = fcmpl
TEST_TARGET = $(BUILDDIR)/$(TARGET)_test

SOURCES = $(wildcard $(SRCDIR)/*.c) $(wildcard $(LIBDIR)/*.c)
OBJECTS = $(addprefix $(BUILDDIR)/, $(notdir $(SOURCES:.c=.o)))

$(shell mkdir -p $(BINDIR) $(BUILDDIR))

ifeq ($(DEBUG), true)
	CFLAGS += $(DEBUGFLAGS)
endif

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(LIBDIR) -c $< -o $@

$(BUILDDIR)/%.o: $(LIBDIR)/%.c
	$(CC) $(CFLAGS) -I$(LIBDIR) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

.PHONY: test
test:
	$(CC) $(CFLAGS) -D DEBUG -I$(LIBDIR) $(LIBDIR)/trie.c $(TESTDIR)/trie_test.c -o $(TEST_TARGET) && ./$(TEST_TARGET)
