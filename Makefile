-include Makefile.rules

CC=g++
CFILES=src/main.cpp \
	src/json.cpp \
	src/parser.cpp \
	src/linked_lists.cpp

OBJS=${CFILES:.c=.o}

all: json-parser
	./json-parser r.json

.PHONY:
json-parser: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o json-parser

clean:
	rm json-parser
