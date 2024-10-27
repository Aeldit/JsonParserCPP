-include Makefile.rules

CC=g++
CFILES=src/main.cpp \
	src/json.cpp \
	src/parser.cpp \
	src/json_types.cpp

ADDITIONAL_FLAGS= \
				 #-DVALGRING_DISABLE_PRINT

all: clean json-parser-cpp
	./json-parser-cpp r.json

.PHONY:
json-parser-cpp:
	$(CC) $(CFLAGS) $(ADDITIONAL_FLAGS) $(CFILES) -o json-parser-cpp

clean:
	if [ -f "json-parser-cpp" ]; then rm json-parser-cpp; fi

valgrind-compile: clean
	$(CC) $(CFLAGS) \
		-DVALGRING_DISABLE_PRINT \
		$(CFILES) -o json-parser-cpp

valgrind: valgrind-compile
	valgrind --tool=callgrind --dump-instr=yes \
		--simulate-cache=yes --collect-jumps=yes ./json-parser-cpp big.json

leaks: valgrind-compile
	valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes ./json-parser-cpp r.json
