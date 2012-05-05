## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_mem.la

libmod_mem_la_SOURCES = mod_mem.c
libmod_mem_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
libmod_mem_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_mem_la_LIBADD = $(COMMON_LDFLAGS)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_mem.la

