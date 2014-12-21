# LLVM clang compiler specific config
$(info "Using clang")
LN=clang
# PROFILER_CC_OPTIONS=-pg
# PROFILER_LN_OPTIONS=-pg
DEBUG_OPTIONS=-g $(PROFILER_CC_OPTIONS)
CFLAGS_NO_ANSI=-Wstrict-prototypes --pedantic -Wall $(DEBUG_OPTIONS) -march=native
CFLAGS= $(CFLAGS_NO_ANSI) --ansi
LDFLAGS= $(DEBUG_OPTIONS) -lm -lrt
