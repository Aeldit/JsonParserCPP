-include Makefile.rules

CC=g++
CFILES=src/main.cpp \
	src/json.cpp \
	src/parser.cpp \
	src/linked_lists.cpp

all: json-parser
	./json-parser r.json

.PHONY:
json-parser:
	$(CC) $(CFLAGS) $(CFILES) -o json-parser

clean:
	rm json-parser
