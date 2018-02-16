#//tb/1802

default: programs
programs: clean dstub blobit
dstub: dstub.c
	cc -o $@ $<
blobit: blobit.c
	cc -o $@ $<
clean:
	-rm -f dstub blobit
