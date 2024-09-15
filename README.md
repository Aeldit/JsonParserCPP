# JSON Parser C++


![json-parser-cpp](https://raw.githubusercontent.com/Aeldit/Aeldit/e5cba7b19560c64d7677915d44ac5e4c80e8ce48/github_profile/jsonparser-cpp.svg)

This JSON parser was created from scratch, the parsing part being the same as my pure C version of
this project.

## Build

To build the program, first clone it from GitHub :

```shell
git clone https://github.com/Aeldit/JsonParserCPP.git
```

Then, enter the following commands

```shell
./configure
make json-parser
./json-parser <your_json_file.json>
```

The configure script accepts the following options :
- S : Runs the script with the `-fsanitize=address` g++ flag (checks for memory leaks)
- D : Displays some debug informations
- SD or DS : Use both options


## Makefile rules

Base rules :
- all : compiles and runs the program with the file `r.json`
- clean : removes the executable (this rule is called by all the other except `tests`)
- test : runs some miscellaneous tests

Valgrind rules :
- valgrind-compile : compiles the parser with the `-DVALGRING_DISABLE_PRINT` flag (disables the printing functions to only have the time of the parsing functions when using a profiler)
- valgrind : calls `valgrind-compile` and generates a callgrind file usable by the `KCachegrind` profiling software
- leaks : checks for leaks but using valgrind (using the file `r.json`)

## Compilation options

You can change some defines directly at compilation time, depending on your use of this parser

### READ_BUFF_SIZE

Defines the size of the local buffers that are used when using buffered reading (defaults to 1024)

### MAX_NESTED_ARRAYS

Defines the maximum number of nested arrays (defaults to `UINT_FAST8_MAX`)

If you want to change this, you can use the following additional flags :
- `-DMAX_NESTED_ARRAYS=UINT_FAST16_MAX`
- `-DMAX_NESTED_ARRAYS=UINT_FAST32_MAX`
- `-DMAX_NESTED_ARRAYS=UINT_FAST64_MAX`

### MAX_NESTED_DICTS

Defines the maximum number of nested dict objects (defaults to `UINT_FAST8_MAX`)

If you want to change this, you can use the following additional flags :
- `-DMAX_NESTED_DICTS=UINT_FAST16_MAX`
- `-DMAX_NESTED_DICTS=UINT_FAST32_MAX`
- `-DMAX_NESTED_DICTS=UINT_FAST64_MAX`

