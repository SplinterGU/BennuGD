## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libjoy.la

libjoy_la_SOURCES = libjoy.c
libjoy_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include
libjoy_la_LDFLAGS = -s -module -no-undefined -avoid-version
libjoy_la_LIBADD = $(COMMON_LDFLAGS) -lSDL -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libjoy.la

