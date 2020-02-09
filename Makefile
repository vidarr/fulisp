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

include config.mk

.PHONY: all fulisp tests check clean preprocess tarball release

all:
	cd src && make all

fulisp:
	cd src && make fulisp

tests: all
	cd src && make tests; \
	cd ../tests && make tests

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


