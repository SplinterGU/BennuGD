## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libwm.la

libwm_la_SOURCES = libwm.c
libwm_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include
libwm_la_LDFLAGS = -s -module -no-undefined -avoid-version
libwm_la_LIBADD = $(COMMON_LDFLAGS) -lSDL -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libwm.la
