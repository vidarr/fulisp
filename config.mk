# (C) 2020 Michael J. Beer
# All rights reserved.
#
# Redistribution  and use in source and binary forms, with or with‐
# out modification, are permitted provided that the following  con‐
# ditions are met:
#
# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above  copy‐
# right  notice,  this  list  of  conditions and the following dis‐
# claimer in the documentation and/or other materials provided with
# the distribution.
#
# 3.  Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote  products  derived
# from this software without specific prior written permission.
#
# THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBU‐
# TORS "AS IS" AND ANY EXPRESS OR  IMPLIED  WARRANTIES,  INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT
# SHALL  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DI‐
# RECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS IN‐
# TERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY  THEORY  OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING  NEGLI‐
# GENCE  OR  OTHERWISE)  ARISING  IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

VERSION=0.4.2

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

