## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmouse.la

libmouse_la_SOURCES = libmouse.c
libmouse_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I. -I../libdraw/ -I../librender/ -I../libblit/ -I../libvideo/ -I../libgrbase/
libmouse_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmouse_la_LIBADD = $(COMMON_LDFLAGS) -lSDL -L$(BGD_PATH)/bgdrtm/src/.libs -L../libdraw/.libs -ldraw -L../librender/.libs -lrender -L../libblit/.libs -lblit -L../libvideo/.libs -lvideo -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmouse.la
