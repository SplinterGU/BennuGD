## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_video.la

libmod_video_la_SOURCES = mod_video.c
libmod_video_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../librender/ -I../libvideo/ -I../libgrbase/
libmod_video_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_video_la_LIBADD = $(COMMON_LDFLAGS) $(SDL_LIBS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../librender/.libs -lrender -L../libvideo/.libs -lvideo -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_video.la
