#!/usr/bin/env bash
# (c) 2020 michael j. beer
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
#
include ../config.mk

INCLUDE_DIR=../src
SOURCE_DIR=../src

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
	$(CALL_PREFIX)$(BUILD_DIR)/reader


$(BUILD_DIR)/tests_reader.o: reader.c $(ABS_HEADER_FILES)
	$(CALL_PREFIX)echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS) -I . -I $(INCLUDE_DIR) -o $@ -c $<


$(BUILD_DIR)/tests_repl.o: repl.c $(ABS_HEADER_FILES)
	$(CALL_PREFIX)echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS) -I . -I $(INCLUDE_DIR) -o $@ -c $<


$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.c $(ABS_HEADER_FILES)
	$(CALL_PREFIX)echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS) -I . -I $(INCLUDE_DIR) -o $@ -c $<
    

$(BUILD_DIR)/tests_reader: $(OBJECT_FILES) $(BUILD_DIR)/tests_reader.o
	$(CALL_PREFIX)echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/tests_reader  $^ $(LDFLAGS)


$(BUILD_DIR)/tests_repl: $(OBJECT_FILES) $(BUILD_DIR)/tests_repl.o
	$(CALL_PREFIX)echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $(BUILD_DIR)/tests_repl  $^ $(LDFLAGS)


clean:
	$(CALL_PREFIX)echo "REMOVING BINARIES..."; rm -f  $(BUILD_DIR)/* # $(OBJECT_FILES) $(BUILD_DIR)/*


tags: $(wildcard *.c) $(wildcard *.h)
	$(CALL_PREFIX)ctags -R *.c *.h > $(TAGS_FILE)

