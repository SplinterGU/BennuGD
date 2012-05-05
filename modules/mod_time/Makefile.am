## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_time.la

libmod_time_la_SOURCES = mod_time.c
libmod_time_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include
libmod_time_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_time_la_LIBADD = $(COMMON_LDFLAGS) $(SDL_LIBS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_time.la

