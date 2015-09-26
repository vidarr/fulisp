RELEASE_DIR=release

.PHONY: all fulisp tests check clean preprocess tarball release

all: 
	cd src && make all

fulisp:
	cd src && make fulisp

tests:
	cd src && make tests

check: 
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


