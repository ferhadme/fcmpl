CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic
DEBUG_FLAG = -D DEBUG

LIB_SRC = lib/trie.c
LIB_OBJ = trie.o
LIB_INCLUDE_FLAG = $(addprefix -I,$(LIB_DIR))
SRC = src/main.c src/repl.c
TEST = tests/trie_test.c
BUILD = build

PNAME = fcmpl
TARGET_EXEC = $(BUILD)/$(PNAME)
TEST_TARGET = $(BUILD)/$(PNAME)
MKDIR_P = mkdir -p

debug: $(LIB_OBJ)
	$(CC) $(DEBUG_FLAG) $(CFLAGS) -o $(TARGET_EXEC) $(SRC) $(DEPS)/$(LIB_OBJ_NAME)

release:
	$(CC) $(LIB_FLAG) $(CFLAGS) -o $(TARGET_EXEC) $(SRC)

test:
	$(MKDIR_P) $(BUILD) $(DEPS)
	$(CC) $(LIB_FLAG) $(DEBUG_FLAG) $(CFLAGS) -o $(TEST_TARGET) $(TEST)
	./fcmpl_test

$(LIB_OBJ):
	$(CC) -c $(LIB_SRC) -o $(DEPS)/$(LIB_OBJ)

$(DEPS): $(BUILD)
	$(MKDIR_P) $(BUILD)/deps

$(BUILD):
	$(MKDIR_P) $(BUILD)

clean:
	rm -rf $(BUILD)/*
