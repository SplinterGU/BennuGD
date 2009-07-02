## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_sound.la

mod_sound_la_SOURCES = mod_sound.c
mod_sound_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include
mod_sound_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_sound_la_LIBADD = $(COMMON_LDFLAGS) -lSDL -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -lSDL_mixer

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_sound.la

