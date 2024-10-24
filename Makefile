-include Makefile.rules

CC=g++
CFILES=src/main.cpp \
	src/json.cpp \
	src/parser.cpp \
	src/json_types.cpp

ADDITIONAL_FLAGS= \
				 #-DVALGRING_DISABLE_PRINT

all: clean json-parser
	./json-parser r.json

.PHONY:
json-parser:
	$(CC) $(CFLAGS) $(ADDITIONAL_FLAGS) $(CFILES) -o json-parser

clean:
	if [ -f "json-parser" ]; then rm json-parser; fi

.PHONY:
test:
	$(CC) $(CFLAGS) tests/tests.cpp -o tests/run-tests
	./tests/run-tests

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
