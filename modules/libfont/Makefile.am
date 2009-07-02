## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libfont.la

libfont_la_SOURCES = libfont.c
libfont_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libgrbase/
libfont_la_LDFLAGS = -s -module -no-undefined -avoid-version
libfont_la_LIBADD = $(COMMON_LDFLAGS) -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libfont.la
