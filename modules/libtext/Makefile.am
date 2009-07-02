## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libtext.la

libtext_la_SOURCES = libtext.c
libtext_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libfont/ -I../librender/ -I../libblit/ -I../libgrbase/
libtext_la_LDFLAGS = -s -module -no-undefined -avoid-version
libtext_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../libfont/.libs -lfont -L../librender/.libs -lrender -L../libblit/.libs -lblit -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libtext.la
