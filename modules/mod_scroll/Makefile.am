## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_scroll.la

mod_scroll_la_SOURCES = mod_scroll.c
mod_scroll_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I../libscroll -I../libvideo -I../libgrbase/
mod_scroll_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_scroll_la_LIBADD = $(COMMON_LDFLAGS) -L../libscroll/.libs -lscroll

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_scroll.la

