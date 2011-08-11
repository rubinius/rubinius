#
# Automake definitions for ud/tests
#
check_PROGRAMS = gen ovrrun

gen_SOURCES    = gen.c
gen_LDADD      = ../libudis86/libudis86.la
gen_CFLAGS     = -I$(top_srcdir)/libudis86 -I$(top_srcdir)
ovrrun_SOURCES = bufovrrun.c
ovrrun_LDADD   = ../libudis86/libudis86.la
ovrrun_CFLAGS  = -I$(top_srcdir)/libudis86 -I$(top_srcdir)

YASM = yasm

dist_check_DATA = test16.asm test32.asm test64.asm testjmp.asm testobscure.asm \
		 testjmp.ref testobscure.ref randtest.raw randtest16.ref      \
		 randtest32.ref randtest64.ref

check-local: tests

tests: test16 test32 test64 testjmp bufovrrun randraw

test16: gen 
	$(YASM) -f bin -o test16.bin $(top_srcdir)/tests/test16.asm
	echo "[bits 16]" > test16.out
	./gen -16 < test16.bin >> test16.out
	diff $(top_srcdir)/tests/test16.asm test16.out

test32: gen
	$(YASM) -f bin -o test32.bin $(top_srcdir)/tests/test32.asm
	echo "[bits 32]" > test32.out
	./gen -32 < test32.bin >> test32.out
	diff $(top_srcdir)/tests/test32.asm test32.out

test64: gen
	$(YASM) -f bin -o test64.bin $(top_srcdir)/tests/test64.asm
	echo "[bits 64]" > test64.out
	./gen -64 < test64.bin >> test64.out
	diff $(top_srcdir)/tests/test64.asm test64.out

testjmp: gen
	$(YASM) -f bin -o testjmp.bin $(top_srcdir)/tests/testjmp.asm
	../udcli/udcli -64 < testjmp.bin > testjmp.out
	diff $(top_srcdir)/tests/testjmp.ref testjmp.out

testobscure: gen
	$(YASM) -f bin -o testobscure.bin $(top_srcdir)/tests/testobscure.asm
	./gen -32 < testobscure.bin > testobscure.out
	diff $(top_srcdir)/tests/testobscure.ref testobscure.out

bufovrrun: ovrrun
	./ovrrun

randraw: 
	./gen -16 < $(top_srcdir)/tests/randtest.raw > randtest16.out
	diff randtest16.out $(top_srcdir)/tests/randtest16.ref
	./gen -32 < $(top_srcdir)/tests/randtest.raw > randtest32.out
	diff randtest32.out $(top_srcdir)/tests/randtest32.ref
	./gen -64 < $(top_srcdir)/tests/randtest.raw > randtest64.out
	diff randtest64.out $(top_srcdir)/tests/randtest64.ref

installcheck-local:
	$(CC) $(srcdir)/gen.c -o gen_installcheck.bin -I$(includedir) -L$(libdir) -ludis86

clean-local:
	rm -f *.bin *.out
