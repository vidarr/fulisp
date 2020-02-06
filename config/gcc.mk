# GNU gcc compiler specific config
$(info "Using GCC")
LN=gcc
# PROFILER_CC_OPTIONS=-pg -fprofile-arcs -ftest-coverage
# PROFILER_LN_OPTIONS=-pg
DEBUG_OPTIONS=-g $(PROFILER_CC_OPTIONS)
CFLAGS_NO_ANSI=-Wstrict-prototypes --pedantic -Wall $(DEBUG_OPTIONS) -march=native
CFLAGS= $(CFLAGS_NO_ANSI) --ansi
LDFLAGS= $(DEBUG_OPTIONS) -lm -lrt
