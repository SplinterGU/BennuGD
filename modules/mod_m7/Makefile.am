## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_m7.la

libmod_m7_la_SOURCES = mod_m7.c
libmod_m7_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I../librender/ -I../libblit/ -I../libvideo/ -I../libgrbase/
libmod_m7_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_m7_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../libvideo/.libs -lvideo -L../librender/.libs -lrender -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_m7.la
