## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_grproc.la

mod_grproc_la_SOURCES = mod_grproc.c
mod_grproc_la_CFLAGS = $(COMMON_CFLAGS) $(SDL_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../librender/ -I../libblit/ -I../libvideo/ -I../libgrbase/ -I../libscroll/
mod_grproc_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_grproc_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm -L../librender/.libs -lrender -L../libblit/.libs -lblit -L../libvideo/.libs -lvideo -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_grproc.la
