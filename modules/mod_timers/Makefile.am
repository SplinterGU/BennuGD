## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_timers.la

mod_timers_la_SOURCES = mod_timers.c
mod_timers_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include
mod_timers_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_timers_la_LIBADD = $(COMMON_LDFLAGS) -lSDL -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_timers.la

