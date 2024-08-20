-include Makefile.rules

CC=g++
CFILES=src/main.cpp \
	src/json.cpp

OBJS=${CFILES:.c=.o}

JSONFILES=*.json

all: json-parser
	./json-parser

.PHONY:
json-parser: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o json-parser

lib:
	$(CC) $(CFLAGS) -c $(CFILES) -fpic
	$(CC) $(CFLAGS) *.o -shared -o libjson-parser.so
	rm *.o

clean:
	rm json-parser
