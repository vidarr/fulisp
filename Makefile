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

include config.mk

.PHONY: all fulisp tests check clean preprocess tarball release

all:
	cd src && make all

fulisp:
	cd src && make fulisp

tests: all
	make -C src tests; \
		make -C tests

check: tests
	cd src && make check

clean:
	echo "REMOVING $(RELEASE_DIR) ..."
	if [ -d $(RELEASE_DIR) ]; then  rm -r $(RELEASE_DIR); fi; \
		cd src && make clean && \
		cd ../doc && make clean

preprocess:
	cd src && make preprocess

tarball:
	git archive --prefix=fulisp/ --format tar HEAD | gzip > fulisp.tar.gz

release: tarball
	bash bin/release.sh $(RELEASE_DIR)

print:
	echo "root $(CFLAGS)"; \
		cd src && make src print

