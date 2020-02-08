CC=gcc
LN=gcc
# PROFILER_CC_OPTIONS=-pg
# PROFILER_LN_OPTIONS=-pg
DEBUG_OPTIONS=-g $(PROFILER_CC_OPTIONS)
CFLAGS=-Wstrict-prototypes --pedantic --ansi -Wall $(DEBUG_OPTIONS)
LDFLAGS=-g $(PROFILER_LN_OPTIONS)
INCLUDE_DIR=../src
SOURCE_DIR=../src

BUILD_DIR=../build

TAGS_FILE=tags

SOURCE_FILES= cons.c error.c expression.c fulispreader.c hash.c nativefunctions.c print.c reader.c safety.c streams.c symboltable.c  test.c environment.c eval.c stack.c lambda.c

HEADER_FILES= config.h cons.h error.h expression.h fulispreader.h hash.h nativefunctions.h print.h reader.h safety.h streams.h symboltable.h test.h environment.h eval.h stack.h lambda.h

OBJECT_FILES=$(addprefix $(BUILD_DIR)/, $(SOURCE_FILES:.c=.o))
ABS_HEADER_FILES=$(addprefix $(INCLUDE_DIR)/, $(HEADER_FILES))
ABS_SOURCE_FILES=$(addprefix $(SOURCE_DIR)/, $(SOURCE_FILES))


all: tests


.PHONY: clean tests exec-tests repl


tests: $(BUILD_DIR)/tests_reader


repl: $(BUILD_DIR)/tests_repl


exec-tests: tests
	@$(BUILD_DIR)/reader


$(BUILD_DIR)/tests_reader.o: reader.c $(ABS_HEADER_FILES)
	@echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS) -I . -I $(INCLUDE_DIR) -o $@ -c $<


$(BUILD_DIR)/tests_repl.o: repl.c $(ABS_HEADER_FILES)
	@echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS) -I . -I $(INCLUDE_DIR) -o $@ -c $<


$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.c $(ABS_HEADER_FILES)
	@echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS) -I . -I $(INCLUDE_DIR) -o $@ -c $<
    

$(BUILD_DIR)/tests_reader: $(OBJECT_FILES) $(BUILD_DIR)/tests_reader.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/tests_reader  $^ $(LDFLAGS)


$(BUILD_DIR)/tests_repl: $(OBJECT_FILES) $(BUILD_DIR)/tests_repl.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/tests_repl  $^ $(LDFLAGS)


clean:
	@echo "REMOVING BINARIES..."; rm -f  $(BUILD_DIR)/* # $(OBJECT_FILES) $(BUILD_DIR)/*


tags: $(wildcard *.c) $(wildcard *.h)
	ctags -R *.c *.h > $(TAGS_FILE)

