## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_wm.la

libmod_wm_la_SOURCES = mod_wm.c
libmod_wm_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I../libvideo/ -I../libgrbase/
libmod_wm_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_wm_la_LIBADD = $(COMMON_LDFLAGS) $(SDL_LIBS) $(X11LIBS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../libvideo/.libs -lvideo -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_wm.la

