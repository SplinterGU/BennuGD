## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libscroll.la

libscroll_la_SOURCES = libscroll.c
libscroll_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I. -I../librender/ -I../libblit/ -I../libvideo/ -I../libgrbase/
libscroll_la_LDFLAGS = -s -module -no-undefined -avoid-version
libscroll_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../librender/.libs -lrender -L../libblit/.libs -lblit -L../libvideo/.libs -lvideo -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libscroll.la
