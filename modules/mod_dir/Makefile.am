## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_dir.la

libmod_dir_la_SOURCES = mod_dir.c
libmod_dir_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
libmod_dir_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_dir_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_dir.la

