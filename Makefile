CC=gcc
CCOPTS=-Wall -O2
CCFLAGS=-std=c99
DEBUG=-g -D=DEBUG

LIB_NAME=hashmap
SRC_LIB=src/$(LIB_NAME).c
SRC_TEST=src/main.c
BIN=$(LIB_NAME).o
DEST=lib$(LIB_NAME).a
TEST=main
HEADERS=src/$(LIB_NAME).h

all: $(DEST)

debug: CCFLAGS += $(DEBUG)
debug: $(DEST)

test: $(DEST) $(TEST)

$(DEST): $(SRC_LIB) $(HEADERS) Makefile
	$(CC) -c $(CCOPTS) $(SRC_LIB) -o $(BIN) $(CCFLAGS)
	ar rcs $(DEST) $(BIN)

$(TEST): $(SRC_TEST) $(HEADERS) Makefile
	$(CC) $(CCOPTS) $(SRC_TEST) -o $(TEST) $(CCFLAGS) -L. -l$(LIB_NAME)

clean:
	rm $(BIN) $(DEST) $(TEST)
