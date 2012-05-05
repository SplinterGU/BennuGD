## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = libmod_math.la

libmod_math_la_SOURCES = mod_math.c
libmod_math_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
libmod_math_la_LDFLAGS = -s -module -no-undefined -avoid-version
libmod_math_la_LIBADD = $(COMMON_LDFLAGS) -lm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = libmod_math.la

