## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libsdlhandler.la

libsdlhandler_la_SOURCES = libsdlhandler.c
libsdlhandler_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include
libsdlhandler_la_LDFLAGS = -s -module -no-undefined -avoid-version
libsdlhandler_la_LIBADD = $(COMMON_LDFLAGS) -lSDL

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libsdlhandler.la

