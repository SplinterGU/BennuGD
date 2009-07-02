## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libdraw.la

libdraw_la_SOURCES = libdraw.c
libdraw_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libgrbase/ -I../libblit/
libdraw_la_LDFLAGS = -s -module -no-undefined -avoid-version
libdraw_la_LIBADD = $(COMMON_LDFLAGS) -L../libblit/.libs -lblit -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libdraw.la
