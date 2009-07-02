## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libgrbase.la

libgrbase_la_SOURCES = g_bitmap.c g_clear.c g_grlib.c g_pal.c g_blendop.c g_conversion.c libgrbase.c
libgrbase_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include -I.
libgrbase_la_LDFLAGS = -s -module -no-undefined -avoid-version
libgrbase_la_LIBADD = $(COMMON_LDFLAGS)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libgrbase.la
