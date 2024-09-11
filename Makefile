-include Makefile.rules

CC=g++
CFILES=src/main.cpp \
	src/json.cpp \
	src/parser.cpp \
	src/types.cpp

all: clean json-parser
	./json-parser r.json

.PHONY:
json-parser:
	$(CC) $(CFLAGS) $(CFILES) -o json-parser

clean:
	if [ -f "json-parser" ]; then rm json-parser; fi

valgrind-compile: clean
	$(CC) $(CFLAGS) \
		-DVALGRING_DISABLE_PRINT \
		$(CFILES) -o json-parser

valgrind: valgrind-compile
	valgrind --tool=callgrind --dump-instr=yes \
		--simulate-cache=yes --collect-jumps=yes ./json-parser big.json

leaks: valgrind-compile
	valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes ./json-parser r.json
