## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_screen.la

libmod_screen_la_SOURCES = mod_screen.c
libmod_screen_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libvideo/ -I../librender/ -I../libblit/ -I../libgrbase/ -I../libscroll/
libmod_screen_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_screen_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../libvideo/.libs -lvideo -L../librender/.libs -lrender -L../libblit/.libs -lblit -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_screen.la
