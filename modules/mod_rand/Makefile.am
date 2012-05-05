## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_rand.la

libmod_rand_la_SOURCES = mod_rand.c
libmod_rand_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
libmod_rand_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_rand_la_LIBADD = $(COMMON_LDFLAGS)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_rand.la

