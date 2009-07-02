## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_wm.la

mod_wm_la_SOURCES = mod_wm.c
mod_wm_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I../libvideo/ -I../libgrbase/
mod_wm_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_wm_la_LIBADD = $(COMMON_LDFLAGS) -lSDL $(X11LIBS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../libvideo/.libs -lvideo -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_wm.la

