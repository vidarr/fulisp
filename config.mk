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

VERSION=0.4.1

# For debug, best way is to source in debug.sh in your compile shell and modify
# the CFLAGS export
CFLAGS+=-Wstrict-prototypes --pedantic -Wall -march=native -ansi -fPIC
CFLAGS_NO_FLAGS= $(CFLAGS) -Wstrict-prototypes --pedantic -Wall -march=native -fPIC
LDFLAGS+=-lm -lrt

MKDIR=mkdir -p

LAUNCHER=
BUILD_DIR=../build
PREPROCESS_DIR=../preprocessed
TAGS_FILE=tags

ifdef SILENT
	CALL_PREFIX=@
endif


ifndef CC
	CC=gcc
endif

ifndef LN
	LN=gcc
endif

