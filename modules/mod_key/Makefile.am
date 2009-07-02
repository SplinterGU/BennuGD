## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_key.la

mod_key_la_SOURCES = mod_key.c
mod_key_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I../libkey
mod_key_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_key_la_LIBADD = $(COMMON_LDFLAGS) -L../libkey/.libs -lkey -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_key.la

