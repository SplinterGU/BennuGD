## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_key.la

libmod_key_la_SOURCES = mod_key.c
libmod_key_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I../libkey
libmod_key_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_key_la_LIBADD = $(COMMON_LDFLAGS) -L../libkey/.libs -lkey -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_key.la

