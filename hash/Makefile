
#make|grep "^;" >out.csv

default: test

test:
	@cd MD5 && ./test.sh
	@cd SHA-1 && ./test.sh
	@cd SHA-2 && ./test.sh

clean:
	cd MD5 && make clean
	cd SHA-1 && make clean
	cd SHA-2 && make clean

