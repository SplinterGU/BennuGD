## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_blendop.la

mod_blendop_la_SOURCES = mod_blendop.c
mod_blendop_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libgrbase/
mod_blendop_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_blendop_la_LIBADD = $(LIBADD_DL) -L../libgrbase/.libs -lgrbase

#mod_blendop_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libfont/ -I../libblit/ -I../libvideo/ -I../libgrbase/
#mod_blendop_la_LDFLAGS = -s -module -no-undefined -avoid-version -lpng
#mod_blendop_la_LIBADD = $(LIBADD_DL) -lSDL -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../libfont/.libs -lfont -L../libblit/.libs -lblit -L../libvideo/.libs -lvideo -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_blendop.la
