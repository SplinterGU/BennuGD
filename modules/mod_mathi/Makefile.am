## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_mathi.la

libmod_mathi_la_SOURCES = mod_mathi.c
libmod_mathi_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
libmod_mathi_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_mathi_la_LIBADD = $(COMMON_LDFLAGS) -lm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_mathi.la

