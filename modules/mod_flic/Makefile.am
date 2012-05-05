## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_flic.la

libmod_flic_la_SOURCES = mod_flic.c
libmod_flic_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I. -I../librender/ -I../libblit/ -I../libgrbase/
libmod_flic_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_flic_la_LIBADD = $(COMMON_LDFLAGS) $(SDL_LIBS) -L$(BGD_PATH)/bgdrtm/src/.libs -L../librender/.libs -lrender -L../libblit/.libs -lblit -L../libgrbase/.libs -lgrbase -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_flic.la

