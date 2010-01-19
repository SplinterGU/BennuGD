## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_map.la

mod_map_la_SOURCES = file_pcx.c file_png.c file_map.c file_pal.c file_fpg.c file_fnt.c mod_map.c
mod_map_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../librender/ -I../libfont/ -I../libblit/ -I../libvideo/ -I../libgrbase/
mod_map_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_map_la_LIBADD = $(COMMON_LDFLAGS) $(PNGLIBS) -lSDL -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../librender/.libs -lrender -L../libfont/.libs -lfont -L../libblit/.libs -lblit -L../libvideo/.libs -lvideo -L../libgrbase/.libs -lgrbase -lpng

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_map.la
