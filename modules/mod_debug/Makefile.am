## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_debug.la

mod_debug_la_SOURCES = mod_debug.c systexts.c
mod_debug_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libkey -I../librender/ -I../libgrbase
mod_debug_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_debug_la_LIBADD = $(COMMON_LDFLAGS) -L../libkey/.libs -lkey -L../librender/.libs -lrender -L../libgrbase/.libs -lgrbase -lSDL -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_debug.la

