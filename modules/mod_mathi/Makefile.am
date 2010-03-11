## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_mathi.la

mod_mathi_la_SOURCES = mod_mathi.c
mod_mathi_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
mod_mathi_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_mathi_la_LIBADD = $(COMMON_LDFLAGS) -lm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_mathi.la

