# (C) 2020 Michael J. Beer
#
# this program is free software; you can redistribute it and/or modify
# it under the terms of the gnu general public license as published by
# the free software foundation; either version 3 of the license, or
# (at your option) any later version.
#
# this program is distributed in the hope that it will be useful,
# but without any warranty; without even the implied warranty of
# merchantability or fitness for a particular purpose.  see the
# gnu general public license for more details.
#
# you should have received a copy of the gnu general public license
# along with this program; if not, write to the free software
# foundation, inc., 51 franklin street, fifth floor, boston, ma  02110-1301,
# usa.

include ../config.mk
include ../files.mk

HEADER_FILES += $(VERSION_FILE)

OBJECT_FILES=$(addprefix $(BUILD_DIR)/, $(SOURCE_FILES:.c=.o))

PREPROCESSED_FILES=$(addprefix $(PREPROCESS_DIR)/, $(SOURCE_FILES:.c=.i))



all: $(BUILD_DIR)/fulisp



.PHONY: clean tests check fulisp preprocess


$(VERSION_FILE):
	$(CALL_PREFIX)echo "#define VERSION $(VERSION)" > $VERSION_FILE

fulisp: $(BUILD_DIR)/fulisp


$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)


tests: $(BUILD_DIR)/test-eval $(BUILD_DIR)/test-reader $(BUILD_DIR)/test-stack $(BUILD_DIR)/test-hash $(BUILD_DIR)/test-expression $(BUILD_DIR)/test-env  $(BUILD_DIR)/test-streams $(BUILD_DIR)/test-memory $(BUILD_DIR)/test-gc


check: tests
	$(CALL_REFIX)$(LAUNCHER) $(BUILD_DIR)/test-stack && \
		$(LAUNCHER) $(BUILD_DIR)/test-hash && \
		$(LAUNCHER) $(BUILD_DIR)/test-env && \
		$(LAUNCHER) $(BUILD_DIR)/test-memory && \
		$(LAUNCHER) $(BUILD_DIR)/test-expression && \
		$(LAUNCHER) $(BUILD_DIR)/test-streams && \
		$(LAUNCHER) $(BUILD_DIR)/test-reader && \
		$(LAUNCHER) $(BUILD_DIR)/test-eval && \
		$(LAUNCHER) $(BUILD_DIR)/test-gc


$(BUILD_DIR)/benchmarking.o:benchmarking.c $(HEADER_FILES) $(BUILD_DIR)
	$(CALL_REFIX)echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS_NO_ANSI) -I . -o $@ -c $<

$(BUILD_DIR)/%.o:%.c $(HEADER_FILES) $(BUILD_DIR)
	$(CALL_REFIX)echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS) -I . -o $@ -c $<

$(BUILD_DIR)/test-reader: $(OBJECT_FILES) $(BUILD_DIR)/test-reader.o  $(BUILD_DIR)/testfileinput.o 
	$(CALL_REFIX)echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-reader  $^ $(LDFLAGS)


$(BUILD_DIR)/test-eval: $(OBJECT_FILES)  $(BUILD_DIR)/test-eval.o $(BUILD_DIR)/testfileinput.o
	$(CALL_REFIX)echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-eval  $^ $(LDFLAGS)


$(BUILD_DIR)/test-gc: $(OBJECT_FILES)  $(BUILD_DIR)/test-gc.o
	$(CALL_REFIX)echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-gc  $^ $(LDFLAGS)


$(BUILD_DIR)/test-stack: $(OBJECT_FILES) $(BUILD_DIR)/test-stack.o
	$(CALL_REFIX)echo "Linking \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-stack $^ $(LDFLAGS)


$(BUILD_DIR)/test-hash: $(OBJECT_FILES)  $(BUILD_DIR)/test-hash.o
	$(CALL_REFIX)echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-hash  $^ $(LDFLAGS)


$(BUILD_DIR)/test-expression: $(OBJECT_FILES)  $(BUILD_DIR)/test-expression.o
	$(CALL_REFIX)echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-expression  $^ $(LDFLAGS)


$(BUILD_DIR)/test-env: $(OBJECT_FILES)  $(BUILD_DIR)/test-env.o
	$(CALL_REFIX)echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-env  $^ $(LDFLAGS)


$(BUILD_DIR)/test-streams: $(OBJECT_FILES)  $(BUILD_DIR)/test-streams.o
	$(CALL_REFIX)echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-streams  $^ $(LDFLAGS)


$(BUILD_DIR)/test-memory: $(OBJECT_FILES) $(BUILD_DIR)/test-memory.o
	$(CALL_REFIX)echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/test-memory  $^ $(LDFLAGS)


$(BUILD_DIR)/fulisp: $(OBJECT_FILES) $(BUILD_DIR)/repl.o
	$(CALL_PREFIX)echo "LINKING \033[01;32m$@\033[00m\n";\
		$(LN) -o $(BUILD_DIR)/fulisp $^ $(LDFLAGS)


clean:
	$(CALL_REFIX)echo "REMOVING BINARIES..."; \
		if [ -d $(BUILD_DIR) ]; then  rm -r $(BUILD_DIR); fi; \
		echo "REMOVING PREPROCESSED SOURCES...";  \
		if [ -d $(PREPROCESS_DIR) ]; then rm -r $(PREPROCESS_DIR); fi


tags: $(wildcard *.c) $(wildcard *.h)
	$(CALL_PREFIX)ctags -R *.c *.h > $(TAGS_FILE)

$(PREPROCESS_DIR):
	$(CALL_PREFIX)$(MKDIR) $(PREPROCESS_DIR)

$(PREPROCESS_DIR)/%.i : %.c
	$(CALL_PREFIX)$(CC) -E $< -o $$(CALL_REFIX)

preprocess: $(PREPROCESS_DIR) $(PREPROCESSED_FILES)

print-%:
