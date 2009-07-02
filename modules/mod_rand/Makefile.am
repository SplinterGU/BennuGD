## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies foreign

lib_LTLIBRARIES = mod_rand.la

mod_rand_la_SOURCES = mod_rand.c
mod_rand_la_CFLAGS = $(COMMON_CFLAGS) -I$(BGD_PATH)/include
mod_rand_la_LDFLAGS = -s -module -no-undefined -avoid-version
mod_rand_la_LIBADD = $(COMMON_LDFLAGS)

## Make sure these will be cleaned even when they're not built by
## default.
CLEANFILES = mod_rand.la

