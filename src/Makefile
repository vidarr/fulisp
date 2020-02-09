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
TEST_BINARIES_ABS=$(addprefix $(BUILD_DIR)/, $(TEST_BINARIES))

all: $(BUILD_DIR)/fulisp

.PHONY: clean tests check fulisp preprocess

$(VERSION_FILE):
	$(CALL_PREFIX)echo "#define VERSION $(VERSION)" > $VERSION_FILE

fulisp: $(BUILD_DIR)/fulisp


$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)


tests: $(TEST_BINARIES_ABS)


check: tests
	$(CALL_PREFIX)for test in $(addprefix $(BUILD_DIR)/, $(TEST_BINARIES)); do \
	$(CALL_REFIX)$(LAUNCHER) $(TEST); \
	done


$(BUILD_DIR)/benchmarking.o:benchmarking.c $(HEADER_FILES) $(BUILD_DIR)
	$(CALL_REFIX)echo "COMPILING \033[01;32m$<\033[00m\n"; $(CC) $(CFLAGS_NO_ANSI) -I . -o $@ -c $<

$(BUILD_DIR)/test-%: $(BUILD_DIR)/test-%.o $(OBJECT_FILES)  $(BUILD_DIR)/testfileinput.o
	$(CALL_REFIX)echo "LINKING \033[01;32m$@\033[00m\n"; $(LN) -o $@  $^ $(LDFLAGS)

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
	$(CALL_PREFIX)$(CC) -E $< -o $@

preprocess: $(PREPROCESS_DIR) $(PREPROCESSED_FILES)

print-%:
