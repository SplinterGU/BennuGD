## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_math.la

mod_math_la_SOURCES = mod_math.c
mod_math_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
mod_math_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_math_la_LIBADD = $(COMMON_LDFLAGS) -lm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_math.la

