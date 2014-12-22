#CC=clang
CC=gcc
MKDIR=mkdir
#LAUNCHER=valgrind --leak-check=full
LAUNCHER=

ifeq ($(CC), gcc)
include ../config/gcc.mk
else ifeq ($(CC), clang)
include ../config/clang.mk
else
$(error CC variable not set to supported compiler)
endif

BUILD_DIR=../build

TAGS_FILE=tags

SOURCE_FILES= cons.c error.c expression.c fulispreader.c hash.c nativefunctions.c print.c reader.c safety.c streams.c symboltable.c  test.c environment.c eval.c stack.c lambda.c benchmarking.c memory.c garbage_collector.c

HEADER_FILES= config.h cons.h error.h expression.h fulispreader.h hash.h nativefunctions.h print.h reader.h safety.h streams.h symboltable.h test.h environment.h eval.h stack.h lambda.h benchmarking.h memory.h garbage_collector.h

OBJECT_FILES=$(addprefix $(BUILD_DIR)/, $(SOURCE_FILES:.c=.o))



all: $(BUILD_DIR)/fulisp



.PHONY: clean tests exec-tests fulisp



fulisp: $(BUILD_DIR)/fulisp


$(BUILD_DIR): 
	$(MKDIR) $(BUILD_DIR)


tests: $(BUILD_DIR)/test-eval $(BUILD_DIR)/test-reader $(BUILD_DIR)/test-stack $(BUILD_DIR)/test-hash $(BUILD_DIR)/test-expression $(BUILD_DIR)/test-env  $(BUILD_DIR)/test-streams $(BUILD_DIR)/test-memory $(BUILD_DIR)/test-gc


exec-tests: tests
	@$(LAUNCHER) $(BUILD_DIR)/test-stack && \
	 $(LAUNCHER) $(BUILD_DIR)/test-hash && \
	 $(LAUNCHER) $(BUILD_DIR)/test-memory && \
	 $(LAUNCHER) $(BUILD_DIR)/test-expression && \
     $(LAUNCHER) $(BUILD_DIR)/test-streams && \
	 $(LAUNCHER) $(BUILD_DIR)/test-reader && \
	 $(LAUNCHER) $(BUILD_DIR)/test-eval && \
	 $(LAUNCHER) $(BUILD_DIR)/test-gc 


$(BUILD_DIR)/benchmarking.o:benchmarking.c $(HEADER_FILES)
	@echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS_NO_ANSI) -I . -o $@ -c $<

$(BUILD_DIR)/%.o:%.c $(HEADER_FILES) $(BUILD_DIR)
	@echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS) -I . -o $@ -c $<

$(BUILD_DIR)/test-reader: $(OBJECT_FILES) $(BUILD_DIR)/test-reader.o  $(BUILD_DIR)/testfileinput.o 
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-reader  $^ $(LDFLAGS)


$(BUILD_DIR)/test-eval: $(OBJECT_FILES)  $(BUILD_DIR)/test-eval.o $(BUILD_DIR)/testfileinput.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-eval  $^ $(LDFLAGS)


$(BUILD_DIR)/test-gc: $(OBJECT_FILES)  $(BUILD_DIR)/test-gc.o $(BUILD_DIR)/testfileinput.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-gc  $^ $(LDFLAGS)


$(BUILD_DIR)/test-stack: $(OBJECT_FILES) $(BUILD_DIR)/test-stack.o
	@echo "Linking \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-stack $^ $(LDFLAGS)


$(BUILD_DIR)/test-hash: $(OBJECT_FILES)  $(BUILD_DIR)/test-hash.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-hash  $^ $(LDFLAGS)


$(BUILD_DIR)/test-expression: $(OBJECT_FILES)  $(BUILD_DIR)/test-expression.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-expression  $^ $(LDFLAGS)


$(BUILD_DIR)/test-env: $(OBJECT_FILES)  $(BUILD_DIR)/test-env.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-env  $^ $(LDFLAGS)


$(BUILD_DIR)/test-streams: $(OBJECT_FILES)  $(BUILD_DIR)/test-streams.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-streams  $^ $(LDFLAGS)


$(BUILD_DIR)/test-memory: $(OBJECT_FILES) $(BUILD_DIR)/test-memory.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-memory  $^ $(LDFLAGS)


$(BUILD_DIR)/fulisp: $(OBJECT_FILES) $(BUILD_DIR)/repl.o
	@echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/fulisp $^ $(LDFLAGS)

    
clean:
	@echo "REMOVING BINARIES..."; rm  $(BUILD_DIR)/* # $(OBJECT_FILES) $(BUILD_DIR)/*


tags: $(wildcard *.c) $(wildcard *.h)
	ctags -R *.c *.h > $(TAGS_FILE)

