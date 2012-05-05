## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_sort.la

libmod_sort_la_SOURCES = mod_sort.c
libmod_sort_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
libmod_sort_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_sort_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_sort.la

