## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_debug.la

libmod_debug_la_SOURCES = mod_debug.c systexts.c
libmod_debug_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libkey -I../librender/ -I../libgrbase
libmod_debug_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_debug_la_LIBADD = $(COMMON_LDFLAGS) -L../libkey/.libs -lkey -L../librender/.libs -lrender -L../libgrbase/.libs -lgrbase $(SDL_LIBS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_debug.la

