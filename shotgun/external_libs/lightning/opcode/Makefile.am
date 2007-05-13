EXTRA_LIBRARIES = libdisass.a
noinst_LIBRARIES = @LIBDISASS@

AM_CPPFLAGS = -I$(top_srcdir)

libdisass_a_SOURCES = dis-buf.c i386-dis.c ppc-dis.c ppc-opc.c sparc-dis.c \
	sparc-opc.c disass.c

noinst_HEADERS = ansidecl.h bfd.h dis-asm.h i386.h ppc.h sparc.h sysdep.h
