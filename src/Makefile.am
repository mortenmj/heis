AM_CFLAGS = -std=c99

bin_PROGRAMS = heis
heis_SOURCES = \
	car.c car.h \
    safety.c safety.h \
	ui.c ui.h \
	main.c

heis_LDADD = $(top_builddir)/libheis/libheis.la
LDADD = $(top_builddir)/libheis/libheis.la
