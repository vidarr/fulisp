all: 
	cd src && make all

fulisp:
	cd src && make fulisp

tests:
	cd src && make tests

exec-tests:
	cd src && make exec-tests

clean:
	cd src && make clean

tarball: 
	git archive --format tar HEAD | gzip > fulisp.tar.gz

release: tarball
	bash bin/release.sh


