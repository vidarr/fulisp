all: 
	cd src && make all

fulisp:
	cd src && make fulisp

tests:
	cd src && make tests

check: 
	cd src && make check

clean:
	cd src && make clean

preprocess:
	cd src && make preprocess

tarball: 
	git archive --format tar HEAD | gzip > fulisp.tar.gz

release: tarball
	bash bin/release.sh


