## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_sound.la

libmod_sound_la_SOURCES = mod_sound.c
libmod_sound_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) $(SDL_mixer_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I../libbgload/
libmod_sound_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_sound_la_LIBADD = $(COMMON_LDFLAGS) $(SDL_LIBS) -L$(BGD_PATH)/bgdrtm/src/.libs -L../libbgload/.libs -lbgload -lbgdrtm $(SDL_mixer_LIBS)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_sound.la

