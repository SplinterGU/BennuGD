## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_mouse.la

libmod_mouse_la_SOURCES = mod_mouse.c
libmod_mouse_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include
libmod_mouse_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_mouse_la_LIBADD = $(COMMON_LDFLAGS)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_mouse.la

