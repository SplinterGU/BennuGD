## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_scroll.la

libmod_scroll_la_SOURCES = mod_scroll.c
libmod_scroll_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I../libscroll -I../libvideo -I../libgrbase/
libmod_scroll_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_scroll_la_LIBADD = $(COMMON_LDFLAGS) -L../libscroll/.libs -lscroll

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_scroll.la

