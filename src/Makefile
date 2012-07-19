CC=gcc
LN=gcc

# PROFILER_CC_OPTIONS=-pg
# PROFILER_LN_OPTIONS=-pg
DEBUG_OPTIONS=-g $(PROFILER_CC_OPTIONS)
CFLAGS_NO_ANSI=-Wstrict-prototypes --pedantic -Wall $(DEBUG_OPTIONS)
CFLAGS= $(CFLAGS_NO_ANSI) --ansi
LDFLAGS= -g $(PROFILER_LN_OPTIONS) -lrt -lm

BUILD_DIR=../build

TAGS_FILE=tags

SOURCE_FILES= cons.c error.c expression.c fulispreader.c hash.c nativefunctions.c print.c reader.c safety.c streams.c symboltable.c  test.c environment.c eval.c stack.c lambda.c benchmarking.c

HEADER_FILES= config.h cons.h error.h expression.h fulispreader.h hash.h nativefunctions.h print.h reader.h safety.h streams.h symboltable.h test.h environment.h eval.h stack.h lambda.h benchmarking.h

OBJECT_FILES=$(addprefix $(BUILD_DIR)/, $(SOURCE_FILES:.c=.o))



all: $(BUILD_DIR)/test-eval



.PHONY: clean tests exec-tests fulisp



fulisp: $(BUILD_DIR)/fulisp


tests: $(BUILD_DIR)/test-eval $(BUILD_DIR)/test-reader $(BUILD_DIR)/test-stack $(BUILD_DIR)/test-hash $(BUILD_DIR)/test-expression $(BUILD_DIR)/test-env 


exec-tests: tests
	@$(BUILD_DIR)/test-stack && \
	 $(BUILD_DIR)/test-hash && \
	 $(BUILD_DIR)/test-expression && \
	 $(BUILD_DIR)/test-reader && \
	 $(BUILD_DIR)/test-eval 


perform-tests: tests
	$(BUILD_DIR)/test-hash && $(BUILD_DIR)/test-stack


$(BUILD_DIR)/benchmarking.o:benchmarking.c $(HEADER_FILES)
	@echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS_NO_ANSI) -I . -o $@ -c $<

$(BUILD_DIR)/%.o:%.c $(HEADER_FILES)
	@echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS) -I . -o $@ -c $<

$(BUILD_DIR)/test-reader: $(OBJECT_FILES) $(BUILD_DIR)/test-reader.o  $(BUILD_DIR)/testfileinput.o 
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-reader  $^ $(LDFLAGS)


$(BUILD_DIR)/test-eval: $(OBJECT_FILES)  $(BUILD_DIR)/test-eval.o $(BUILD_DIR)/testfileinput.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-eval  $^ $(LDFLAGS)


$(BUILD_DIR)/test-stack: $(OBJECT_FILES) $(BUILD_DIR)/test-stack.o
	@echo "Linking \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-stack $^ $(LDFLAGS)


$(BUILD_DIR)/test-hash: $(OBJECT_FILES)  $(BUILD_DIR)/test-hash.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-hash  $^ $(LDFLAGS)


$(BUILD_DIR)/test-expression: $(OBJECT_FILES)  $(BUILD_DIR)/test-expression.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-expression  $^ $(LDFLAGS)


$(BUILD_DIR)/test-env: $(OBJECT_FILES)  $(BUILD_DIR)/test-env.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-env  $^ $(LDFLAGS)


$(BUILD_DIR)/fulisp: $(OBJECT_FILES) $(BUILD_DIR)/repl.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/fulisp $^ $(LDFLAGS)

    
clean:
	@echo "REMOVING BINARIES..."; rm  $(BUILD_DIR)/* # $(OBJECT_FILES) $(BUILD_DIR)/*


tags: $(wildcard *.c) $(wildcard *.h)
	ctags -R *.c *.h > $(TAGS_FILE)

