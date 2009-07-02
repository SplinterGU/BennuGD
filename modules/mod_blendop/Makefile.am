## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_blendop.la

mod_blendop_la_SOURCES = mod_blendop.c
mod_blendop_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I$(BGD_PATH)/bgdrtm/include -I. -I../libgrbase/
mod_blendop_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_blendop_la_LIBADD = $(COMMON_LDFLAGS) -L../libgrbase/.libs -lgrbase

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_blendop.la
