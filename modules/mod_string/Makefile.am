## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_string.la

mod_string_la_SOURCES = mod_string.c
mod_string_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
mod_string_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_string_la_LIBADD = $(COMMON_LDFLAGS) -L$(BGD_PATH)/bgdrtm/src/.libs -lbgdrtm

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_string.la

