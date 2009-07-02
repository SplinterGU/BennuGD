## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_draw.la

mod_draw_la_SOURCES = mod_draw.c
mod_draw_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I. -I../libdraw/ -I../librender/ -I../libblit/ -I../libgrbase/
mod_draw_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_draw_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -L../libdraw/.libs -ldraw -L../librender/.libs -lrender -L../libblit/.libs -lblit -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_draw.la

