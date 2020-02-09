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
include ../files.mk

INCLUDE_DIR=../src
SOURCE_DIR=../src

TESTS= test-hash test-env test-memory test-expression test-streams test-reader \
	   test-eval test-gc

OBJECT_FILES=$(addprefix $(BUILD_DIR)/, $(SOURCE_FILES:.c=.o))
ABS_HEADER_FILES=$(addprefix $(INCLUDE_DIR)/, $(HEADER_FILES))

TESTS_FILES=$(addprefix $(BUILD_DIR)/, $TESTS)

all: tests


.PHONY: clean tests check


tests: $(TEST_FILES)


exec-tests: tests
	$(CALL_PREFIX)$(LAUNCHER) $(BUILD_DIR)/reader


$(BUILD_DIR)/tests_%.o: %.c $(ABS_HEADER_FILES)
	$(CALL_PREFIX)echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS) -I . -I $(INCLUDE_DIR) -o $@ -c $<

$(BUILD_DIR)/%.o:$(SOURCE_DIR)/%.c $(ABS_HEADER_FILES)
	$(CALL_PREFIX)echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS) -I . -I $(INCLUDE_DIR) -o $@ -c $<

clean:
	$(CALL_PREFIX)echo "REMOVING BINARIES..."; rm -f  $(BUILD_DIR)/* # $(OBJECT_FILES) $(BUILD_DIR)/*


tags: $(wildcard *.c) $(wildcard *.h)
	$(CALL_PREFIX)ctags -R *.c *.h > $(TAGS_FILE)

check: tests
	$(CALL_REFIX)for test_file in $TEST_FILES; do \
		$(LAUNCHER) $test_file; \
		done
