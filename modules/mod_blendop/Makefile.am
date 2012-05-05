## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_blendop.la

libmod_blendop_la_SOURCES = mod_blendop.c
libmod_blendop_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libgrbase/
libmod_blendop_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_blendop_la_LIBADD = $(COMMON_LDFLAGS) -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_blendop.la
